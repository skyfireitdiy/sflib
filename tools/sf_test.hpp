#pragma once
#include "tools/sf_single_instance.hpp"
#include "tools/sf_test.h"
#include "tools/sf_thread_pool.hpp"

namespace skyfire {

template <typename T>
std::vector<sf_test_func_t__> sf_test_base__<T>::test_data__;

template <typename T>
sf_test_base__<T>::sf_test_base__(const std::string& func_name, std::function<bool()> func)
{
    sf_test_base__<T>::test_data__.push_back(sf_test_func_t__ { func_name, func });
}

template <typename T>
void sf_test_base__<T>::run(int thread_count)
{
    auto pool = sf_thread_pool::make_instance(thread_count);
    std::vector<std::future<bool>> result;
    std::mutex mu;
    for (auto& p : test_data__) {
        result.push_back(pool->add_task(
            [p, &mu]() -> bool {
                auto r = p.func();
                {
                    std::lock_guard<std::mutex> lck(mu);
                    if (r) {
                        std::cout << p.function_name << " passed!" << std::endl;
                        return true;
                    } else {
                        std::cerr << p.function_name << " failed!" << std::endl;
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
    thread_count = pool->thread_count();
    std::cout << std::endl
              << std::endl
              << "thread_count: " << thread_count << " pass: " << count_pass << " failed: " << count_failed << std::endl;
}
}