#pragma once
#include "sf_empty_class.h"
#include "sf_single_instance.h"
#include "sf_stdc++.h"

#define sf_assert(exp, msg)                                                     \
    if (!(exp)) {                                                               \
        std::cerr << __FILE__ << ":" << __LINE__ << " " << __FUNCTION__ << " :" \
                  << " Assert Failure: " << msg << std::endl;                   \
    }

#define sf_should_throw(exp, expection, msg)                                                                             \
    {                                                                                                                    \
        try {                                                                                                            \
            {                                                                                                            \
                exp;                                                                                                     \
            }                                                                                                            \
            std::cerr << __FILE__ << ":" << __LINE__ << " " << __FUNCTION__ << " :"                                      \
                      << " Should throw " << typeid(expection).name() << ", but no throw :" << msg << std::endl;         \
        } catch (const expection& e) {                                                                                   \
        } catch (...) {                                                                                                  \
            std::cerr << __FILE__ << ":" << __LINE__ << " " << __FUNCTION__ << " :"                                      \
                      << " Should throw " << typeid(expection).name() << ", but throw other type :" << msg << std::endl; \
        }                                                                                                                \
    }

#define sf_should_no_throw(exp, msg)                                                \
    {                                                                               \
        try {                                                                       \
            exp;                                                                    \
        } catch (...) {                                                             \
            std::cerr << __FILE__ << ":" << __LINE__ << " " << __FUNCTION__ << " :" \
                      << " Should no throw: " << msg << std::endl;                  \
        }                                                                           \
    }

namespace skyfire {

struct sf_test_func_t__ {
    std::function<void()> before;
    std::function<void()> after;
    std::string function_name;
    std::function<bool()> func;
    std::string class_name;
};

template <typename = sf_empty_class>
class sf_test_base__ {
protected:
    static std::vector<sf_test_func_t__> test_data__;
    static std::unordered_map<std::string, std::function<void()>> setup_func_map__;
    static std::unordered_map<std::string, std::function<void()>> teardown_func_map__;
    static std::function<void()> global_setup__;
    static std::function<void()> global_teardown__;

public:
    sf_test_base__(const std::string& func_name, std::function<bool()> func, std::function<void()> before = nullptr, std::function<void()> after = nullptr);
    template <typename U>
    sf_test_base__(const std::string& func_name, std::function<bool(const U&)> func, const std::vector<U>& data, std::function<void()> before = nullptr, std::function<void()> after = nullptr);
    sf_test_base__(const std::string& func_name, std::function<bool()> func, const std::string& class_name, std::function<void()> before = nullptr, std::function<void()> after = nullptr);
    template <typename U>
    sf_test_base__(const std::string& func_name, std::function<bool(const U&)> func, const std::string& class_name, const std::vector<U>& data, std::function<void()> before = nullptr, std::function<void()> after = nullptr);

    static int run(int thread_count = 1, bool flashing = true);
    static void set_env(const std::string& class_name, std::function<void()> setup, std::function<void()> teardown);
    static void set_global_env(std::function<void()> setup, std::function<void()> teardown);
};

class sf_test_impl__ final : public sf_test_base__<> {
public:
    using sf_test_base__::sf_test_base__;
};

}

#define sf_test(a, ...) skyfire::sf_test_impl__(std::string(#a), std::function(a), ##__VA_ARGS__)
#define sf_test_run(...) skyfire::sf_test_impl__::run(__VA_ARGS__)
#define sf_test_env(...) skyfire::sf_test_impl__::set_env(__VA_ARGS__)
#define sf_test_global_env(...) skyfire::sf_test_impl__::set_global_env(__VA_ARGS__)

