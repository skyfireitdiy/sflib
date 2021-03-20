#pragma once
#include "sf_logger.h"
#include "sf_single_instance.h"
#include "sf_string.h"
#include "sf_table.h"
#include "sf_test.h"
#include "sf_thread_pool.h"
#include "sf_utils.h"
namespace skyfire
{
inline std::mutex& get_test_output_mu__()
{
    static std::mutex mu;
    return mu;
}
inline std::vector<test_data_t__>& get_test_data__()
{
    static std::vector<test_data_t__> test_data__;
    return test_data__;
}
inline std::unordered_map<std::string, std::function<void()>>& get_setup_func_map__()
{
    static std::unordered_map<std::string, std::function<void()>> setup_func_map__;
    return setup_func_map__;
}
inline std::unordered_map<std::string, std::function<void()>>& get_teardown_func_map__()
{
    static std::unordered_map<std::string, std::function<void()>> teardown_func_map__;
    return teardown_func_map__;
}
inline std::function<void()>& get_global_setup__()
{
    static std::function<void()> global_setup__;
    return global_setup__;
}
inline std::function<void()>& get_global_teardown__()
{
    static std::function<void()> global_teardown__;
    return global_teardown__;
}
inline void test_base__(
    const std::string&                  file,
    int                                 line,
    const std::string&                  func_name,
    std::function<void(test_data_t__&)> func,
    const std::string&                  class_name,
    std::function<void()>               before,
    std::function<void()>               after)
{
    get_test_data__().push_back(test_data_t__ { before, after, func_name, func, class_name, file, line, true });
}
template <typename U>
void test_base__(
    const std::string&                            file,
    int                                           line,
    const std::string&                            func_name,
    std::function<void(test_data_t__&, const U&)> func,
    const std::string&                            class_name,
    const std::vector<U>&                         data,
    std::function<void()>                         before,
    std::function<void()>                         after)
{
    for (size_t i = 0; i < data.size(); ++i)
    {
        auto test_data = data[i];
        get_test_data__().push_back(test_data_t__ {
            before,
            after,
            func_name + " " + std::to_string(i),
            [test_data, func](test_data_t__& td) {
                return func(td, test_data);
            },
            class_name,
            file,
            line,
            true });
    }
}
inline void test_set_env(const std::string&    class_name,
                         std::function<void()> setup,
                         std::function<void()> teardown)
{
    get_setup_func_map__()[class_name]    = setup;
    get_teardown_func_map__()[class_name] = teardown;
}
inline void test_set_global_env(std::function<void()> setup, std::function<void()> teardown)
{
    get_global_setup__()    = setup;
    get_global_teardown__() = teardown;
}
inline int run_test(int thread_count, bool flashing)
{
    int  ret        = 0;
    auto start_time = std::chrono::system_clock::now();
    if (get_global_setup__())
    {
        get_global_setup__();
    }
    auto                           pool = thread_pool::make_instance(thread_count);
    std::vector<std::future<void>> result;
    auto&                          test_data = get_test_data__();
    for (auto& p : test_data)
    {
        result.push_back(pool->add_task(
            [&p]() {
                std::function<void()> setup;
                std::function<void()> teardown;
                if (p.class_name != "")
                {
                    try
                    {
                        setup = get_setup_func_map__().at(p.class_name);
                    }
                    catch (std::out_of_range& e)
                    {
                        setup = nullptr;
                    }
                    try
                    {
                        teardown = get_teardown_func_map__().at(p.class_name);
                    }
                    catch (std::out_of_range& e)
                    {
                        teardown = nullptr;
                    }
                }
                if (p.before)
                {
                    p.before();
                }
                if (setup)
                {
                    setup();
                }
                p.func(p);
                if (teardown)
                {
                    teardown();
                }
                if (p.after)
                {
                    p.after();
                }
            }));
    }
    for (auto& p : result)
    {
        p.wait();
    }
    if (get_global_teardown__())
    {
        get_global_teardown__();
    }
    std::vector<color> pass_style { color_fg_green };
    std::vector<color> failed_style { color_fg_red };
    // todo 打印结果
    int   count_pass   = 0;
    int   count_failed = 0;
    table result_tb(4);
    result_tb.set_config({
        {
            table_align::align_center,
        },
        {
            table_align::align_center,
        },
        {
            table_align::align_center,
        },
        {
            table_align::align_center,
        },
    });
    result_tb.set_header({ "class", "case", "file", "result" });
    std::cout << std::endl;
    size_t index = 0;
    for (auto& p : get_test_data__())
    {
        if (p.pass)
        {
            ++count_pass;
            result_tb.add_row({
                " " + p.class_name + " ",
                " " + p.function_name + " ",
                " " + p.file + ":" + std::to_string(p.line) + " ",
                " pass ",
            });
            for (int i = 0; i < 4; ++i)
            {
                result_tb.set_config(index, i, { table_align::align_left, pass_style });
            }
        }
        else
        {
            ++count_failed;
            result_tb.add_row({
                " " + p.class_name + " ",
                " " + p.function_name + " ",
                " " + p.file + ":" + std::to_string(p.line) + " ",
                " failed ",
            });
            for (int i = 0; i < 4; ++i)
            {
                result_tb.set_config(index, i, { table_align::align_left, failed_style });
            }
        }
        ++index;
    }
    std::cout << "Test case execution results:" << std::endl
              << result_tb.to_string() << std::endl;
    get_test_data__().clear();
    if (flashing)
    {
        failed_style = { color_fg_red, color_style_flashing };
    }
    if (count_failed == 0)
    {
        failed_style = pass_style;
    }
    thread_count = pool->thread_count();
    table all_result_tb(4);
    all_result_tb.set_config({
        {
            table_align::align_center,
            {},
            20,
        },
        {
            table_align::align_center,
            {},
            20,
        },
        {
            table_align::align_center,
            pass_style,
            20,
        },
        {
            table_align::align_center,
            failed_style,
            20,
        },
    });
    all_result_tb.set_header({ "thread count", "cost", "pass", "failed" });
    all_result_tb.add_row({ std::to_string(thread_count),
                            string_trim(convert_ns_to_readable(std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now() - start_time).count())), std::to_string(count_pass), std::to_string(count_failed) });
    std::cout << "Statistical results:" << std::endl
              << all_result_tb.to_string() << std::endl;
    return ret;
}
}