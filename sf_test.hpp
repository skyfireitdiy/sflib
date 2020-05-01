#pragma once
#include "sf_colored_string.hpp"
#include "sf_string.hpp"
#include "sf_single_instance.hpp"
#include "sf_test.h"
#include "sf_thread_pool.hpp"
#include "sf_utils.hpp"

namespace skyfire {

template <typename T>
std::vector<sf_test_func_t__> sf_test_base__<T>::test_data__;

template <typename T>
sf_test_base__<T>::sf_test_base__(const std::string& func_name, std::function<bool()> func)
{
    sf_test_base__<T>::test_data__.push_back(sf_test_func_t__ { func_name, func });
}

template <typename T>
void sf_test_base__<T>::run(int thread_count, bool flashing)
{
    auto start_time = std::chrono::system_clock::now();
    auto pool = sf_thread_pool::make_instance(thread_count);
    std::vector<std::future<bool>> result;
    std::mutex mu;
    for (auto& p : test_data__) {
        result.push_back(pool->add_task(
            [p, flashing, &mu]() -> bool {
                auto r = p.func();
                {
                    std::lock_guard<std::mutex> lck(mu);
                    if (r) {
                        std::ostringstream so;
                        so << "thread: " << std::this_thread::get_id() << " " << sf_make_time_str() << " " + p.function_name << " passed!"; 
                        std::cout << sf_colored_string(so.str(), { sf_color_fg_green }) << std::endl;
                        return true;
                    } else {
                        std::vector<sf_color_value> failed_style = { sf_color_fg_red };
                        if (flashing) {
                            failed_style = { sf_color_fg_red, sf_color_style_flashing };
                        }
                        std::ostringstream so;
                        so << "thread: " << std::this_thread::get_id() << " " << sf_make_time_str() << " " << p.function_name << " failed!";
                        std::cerr << sf_colored_string(so.str(), failed_style) << std::endl;
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

    std::vector<sf_color_value> pass_style { sf_color_fg_green };
    std::vector<sf_color_value> failed_style { sf_color_fg_red };
    if (flashing) {
        failed_style = { sf_color_fg_red, sf_color_style_flashing };
    }
    if (count_failed == 0) {
        failed_style = pass_style;
    }

    thread_count = pool->thread_count();
    std::cout << std::endl
              << sf_string::repeat("=", 60)
              << std::endl
              << "thread_count: " << thread_count << std::endl
              << "cost: " << sf_convert_ns_to_readable(std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now() - start_time).count()) << std::endl
              << sf_colored_string("pass: " + std::to_string(count_pass), pass_style) << std::endl
              << sf_colored_string("failed: " + std::to_string(count_failed), failed_style) << std::endl
              << sf_string::repeat("=", 60) << std::endl;
}
}