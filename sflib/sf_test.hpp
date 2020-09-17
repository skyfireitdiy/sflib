#pragma once
#include "sf_color.hpp"
#include "sf_single_instance.hpp"
#include "sf_string.hpp"
#include "sf_table.hpp"
#include "sf_test.h"
#include "sf_thread_pool.hpp"
#include "sf_utils.hpp"

namespace skyfire {

inline std::mutex* sf_get_test_output_mu__()
{
    static std::mutex mu;
    return &mu;
}

inline std::vector<sf_test_func_t__>* sf_get_test_data__()
{
    static std::vector<sf_test_func_t__> test_data__;
    return &test_data__;
}

inline std::unordered_map<std::string, std::function<void()>>* sf_get_setup_func_map__()
{
    static std::unordered_map<std::string, std::function<void()>> setup_func_map__;
    return &setup_func_map__;
}

inline std::unordered_map<std::string, std::function<void()>>* sf_get_teardown_func_map__()
{
    static std::unordered_map<std::string, std::function<void()>> teardown_func_map__;
    return &teardown_func_map__;
}

inline std::function<void()>* sf_get_global_setup__()
{
    static std::function<void()> global_setup__;
    return &global_setup__;
}

inline std::function<void()>* sf_get_global_teardown__()
{
    static std::function<void()> global_teardown__;
    return &global_teardown__;
}

inline void sf_test_base__(
    const std::string& file,
    int line,
    const std::string& func_name,
    std::function<bool()> func,
    const std::string& class_name,
    std::function<void()> before,
    std::function<void()> after)
{
    sf_get_test_data__()->push_back(sf_test_func_t__ { before, after, func_name, func, class_name, file, line });
}

template <typename U>
void sf_test_base__(
    const std::string& file,
    int line,
    const std::string& func_name,
    std::function<bool(const U&)> func,
    const std::string& class_name,
    const std::vector<U>& data,
    std::function<void()> before,
    std::function<void()> after)
{
    for (int i = 0; i < data.size(); ++i) {
        auto test_data = data[i];
        sf_get_test_data__()->push_back(sf_test_func_t__ {
            before,
            after,
            func_name + " " + std::to_string(i),
            [test_data, func]() -> bool {
                return func(test_data);
            },
            class_name,
            file,
            line });
    }
}

inline void sf_test_base__(
    const std::string& file,
    int line,
    const std::string& func_name,
    std::function<bool()> func,
    std::function<void()> before,
    std::function<void()> after)
{
    sf_get_test_data__()->push_back(sf_test_func_t__ { before, after, func_name, func, "", file, line });
}

template <typename U>
void sf_test_base__(
    const std::string& file,
    int line,
    const std::string& func_name,
    std::function<bool(const U&)> func,
    const std::vector<U>& data,
    std::function<void()> before,
    std::function<void()> after)
{
    for (int i = 0; i < data.size(); ++i) {
        auto test_data = data[i];
        sf_get_test_data__()->push_back(sf_test_func_t__ {
            before,
            after,
            func_name + " " + std::to_string(i),
            [test_data, func]() -> bool {
                return func(test_data);
            },
            "",
            file,
            line });
    }
}

inline void sf_test_set_env(const std::string& class_name,
    std::function<void()> setup,
    std::function<void()> teardown)
{
    (*sf_get_setup_func_map__())[class_name] = setup;
    (*sf_get_teardown_func_map__())[class_name] = teardown;
}

inline void sf_test_set_global_env(std::function<void()> setup, std::function<void()> teardown)
{
    *sf_get_global_setup__() = setup;
    *sf_get_global_teardown__() = teardown;
}

inline int sf_test_run(int thread_count, bool flashing)
{
    int ret = 0;
    auto start_time = std::chrono::system_clock::now();
    if (*sf_get_global_setup__()) {
        *sf_get_global_setup__();
    }
    auto pool = sf_thread_pool::make_instance(thread_count);
    std::vector<std::future<bool>> result;
    auto test_data = sf_get_test_data__();
    for (auto& p : *test_data) {
        result.push_back(pool->add_task(
            [p, flashing, &ret]() -> bool {
                std::function<void()> setup;
                std::function<void()> teardown;

                if (p.class_name != "") {
                    try {
                        setup = sf_get_setup_func_map__()->at(p.class_name);
                    } catch (std::out_of_range& e) {
                        setup = nullptr;
                    }
                    try {
                        teardown = sf_get_teardown_func_map__()->at(p.class_name);
                    } catch (std::out_of_range& e) {
                        teardown = nullptr;
                    }
                }

                if (p.before) {
                    p.before();
                }
                if (setup) {
                    setup();
                }
                auto r = p.func();
                if (teardown) {
                    teardown();
                }
                if (p.after) {
                    p.after();
                }
                {
                    std::lock_guard<std::mutex> lck(*sf_get_test_output_mu__());
                    if (r) {
                        std::ostringstream so;
                        so << p.file << ":" << p.line << " Thread: " << std::this_thread::get_id() << " " << sf_make_time_str() << " [" + p.function_name << "] Passed!";
                        std::cout << sf_color_string(so.str(), { sf_color_fg_green }) << std::endl;
                        return true;
                    } else {
                        ret = -1;
                        std::vector<sf_color> failed_style = { sf_color_fg_red };
                        if (flashing) {
                            failed_style = { sf_color_fg_red, sf_color_style_flashing };
                        }
                        std::ostringstream so;
                        so << p.file << ":" << p.line << " Thread: " << std::this_thread::get_id() << " " << sf_make_time_str() << " [" << p.function_name << "] Failed!";
                        std::cerr << sf_color_string(so.str(), failed_style) << std::endl;
                        return false;
                    }
                }
            }));
    }
    sf_get_test_data__()->clear();
    int count_pass = 0;
    int count_failed = 0;
    for (auto& p : result) {
        if (p.get()) {
            ++count_pass;
        } else {
            ++count_failed;
        }
    }

    if (*sf_get_global_teardown__()) {
        *sf_get_global_teardown__();
    }

    std::vector<sf_color> pass_style { sf_color_fg_green };
    std::vector<sf_color> failed_style { sf_color_fg_red };
    if (flashing) {
        failed_style = { sf_color_fg_red, sf_color_style_flashing };
    }
    if (count_failed == 0) {
        failed_style = pass_style;
    }

    thread_count = pool->thread_count();

    sf_table table(4);

    table.set_config({ {
                           sf_table_align::align_center,
                           {},
                           20,
                       },
        {
            sf_table_align::align_center,
            {},
            20,
        },
        {
            sf_table_align::align_center,
            pass_style,
            20,
        },
        {
            sf_table_align::align_center,
            failed_style,
            20,
        } });

    table.set_header({ "thread count", "cost", "pass", "failed" });
    table.add_row({ std::to_string(thread_count),
        sf_string_trim(sf_convert_ns_to_readable(std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now() - start_time).count())), std::to_string(count_pass), std::to_string(count_failed) });

    std::cout << table.to_string() << std::endl;
    return ret;
}
}