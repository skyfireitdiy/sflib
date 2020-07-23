#pragma once
#include "sf_color.hpp"
#include "sf_single_instance.hpp"
#include "sf_string.hpp"
#include "sf_table.hpp"
#include "sf_test.h"
#include "sf_thread_pool.hpp"
#include "sf_utils.hpp"

namespace skyfire {

template <typename T>
std::vector<sf_test_func_t__> sf_test_base__<T>::test_data__;
template <typename T>
std::unordered_map<std::string, std::function<void()>> sf_test_base__<T>::setup_func_map__;
template <typename T>
std::unordered_map<std::string, std::function<void()>> sf_test_base__<T>::teardown_func_map__;

template <typename T>
sf_test_base__<T>::sf_test_base__(const std::string& func_name, std::function<bool()> func, const std::string& class_name, std::function<void()> before, std::function<void()> after)
{
    sf_test_base__<T>::test_data__.push_back(sf_test_func_t__ { before, after, func_name, func, class_name });
}

template <typename T>
template <typename U>
sf_test_base__<T>::sf_test_base__(const std::string& func_name, std::function<bool(const U&)> func, const std::string& class_name, const std::vector<U>& data, std::function<void()> before, std::function<void()> after)
{
    for (int i = 0; i < data.size(); ++i) {
        auto test_data = data[i];
        sf_test_base__<T>::test_data__.push_back(sf_test_func_t__ { before, after, func_name + " " + std::to_string(i), [test_data, func]() -> bool {
                                                                       return func(test_data);
                                                                   },
            class_name });
    }
}

template <typename T>
sf_test_base__<T>::sf_test_base__(const std::string& func_name, std::function<bool()> func, std::function<void()> before, std::function<void()> after)
{
    sf_test_base__<T>::test_data__.push_back(sf_test_func_t__ { before, after, func_name, func, "" });
}

template <typename T>
template <typename U>
sf_test_base__<T>::sf_test_base__(const std::string& func_name, std::function<bool(const U&)> func, const std::vector<U>& data, std::function<void()> before, std::function<void()> after)
{
    for (int i = 0; i < data.size(); ++i) {
        auto test_data = data[i];
        sf_test_base__<T>::test_data__.push_back(sf_test_func_t__ { before, after, func_name + " " + std::to_string(i), [test_data, func]() -> bool {
                                                                       return func(test_data);
                                                                   },
            "" });
    }
}

template <typename T>
void sf_test_base__<T>::set_env(const std::string& class_name, std::function<void()> setup, std::function<void()> teardown)
{
    setup_func_map__[class_name] = setup;
    teardown_func_map__[class_name] = teardown;
}

template <typename T>
int sf_test_base__<T>::run(int thread_count, bool flashing)
{
    int ret = 0;
    auto start_time = std::chrono::system_clock::now();
    auto pool = sf_thread_pool::make_instance(thread_count);
    std::vector<std::future<bool>> result;
    std::mutex mu;
    for (auto& p : test_data__) {
        result.push_back(pool->add_task(
            [p, flashing, &mu, &ret]() -> bool {
                std::function<void()> setup;
                std::function<void()> teardown;

                if (p.class_name != "") {
                    try {
                        setup = setup_func_map__.at(p.class_name);
                    } catch (std::out_of_range& e) {
                        setup = nullptr;
                    }
                    try {
                        teardown = teardown_func_map__.at(p.class_name);
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
                    std::lock_guard<std::mutex> lck(mu);
                    if (r) {
                        std::ostringstream so;
                        so << "thread: " << std::this_thread::get_id() << " " << sf_make_time_str() << " " + p.function_name << " passed!";
                        std::cout << sf_color_string(so.str(), { sf_color_fg_green }) << std::endl;
                        return true;
                    } else {
                        ret = -1;
                        std::vector<sf_color> failed_style = { sf_color_fg_red };
                        if (flashing) {
                            failed_style = { sf_color_fg_red, sf_color_style_flashing };
                        }
                        std::ostringstream so;
                        so << "thread: " << std::this_thread::get_id() << " " << sf_make_time_str() << " " << p.function_name << " failed!";
                        std::cerr << sf_color_string(so.str(), failed_style) << std::endl;
                        return false;
                    }
                }
            }));
    }
    test_data__.clear();
    int count_pass = 0;
    int count_failed = 0;
    for (auto& p : result) {
        if (p.get()) {
            ++count_pass;
        } else {
            ++count_failed;
        }
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