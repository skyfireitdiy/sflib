#pragma once
#include "sf_color.hpp"
#include "sf_empty_class.h"
#include "sf_object_meta_run.hpp"
#include "sf_single_instance.h"
#include "sf_stdc++.h"

#define sf_assert(exp, msg)                                                     \
    if (!(exp))                                                                 \
    {                                                                           \
        std::cerr << __FILE__ << ":" << __LINE__ << " " << __FUNCTION__ << " :" \
                  << " Assert Failure: " << msg << std::endl;                   \
    }

#define sf_should_throw(exp, expection, msg)                                                                             \
    {                                                                                                                    \
        try                                                                                                              \
        {                                                                                                                \
            {                                                                                                            \
                exp;                                                                                                     \
            }                                                                                                            \
            std::cerr << __FILE__ << ":" << __LINE__ << " " << __FUNCTION__ << " :"                                      \
                      << " Should throw " << typeid(expection).name() << ", but no throw :" << msg << std::endl;         \
        }                                                                                                                \
        catch (const expection& e)                                                                                       \
        {                                                                                                                \
        }                                                                                                                \
        catch (...)                                                                                                      \
        {                                                                                                                \
            std::cerr << __FILE__ << ":" << __LINE__ << " " << __FUNCTION__ << " :"                                      \
                      << " Should throw " << typeid(expection).name() << ", but throw other type :" << msg << std::endl; \
        }                                                                                                                \
    }

#define sf_should_no_throw(exp, msg)                                                \
    {                                                                               \
        try                                                                         \
        {                                                                           \
            exp;                                                                    \
        }                                                                           \
        catch (...)                                                                 \
        {                                                                           \
            std::cerr << __FILE__ << ":" << __LINE__ << " " << __FUNCTION__ << " :" \
                      << " Should no throw: " << msg << std::endl;                  \
        }                                                                           \
    }

namespace skyfire
{

struct test_data_t__
{
    std::function<void()>               before;
    std::function<void()>               after;
    std::string                         function_name;
    std::function<void(test_data_t__&)> func;
    std::string                         class_name;
    std::string                         file;
    int                                 line;
    bool                                pass;
};

std::vector<test_data_t__>&                             get_test_data__();
std::unordered_map<std::string, std::function<void()>>& get_setup_func_map__();
std::unordered_map<std::string, std::function<void()>>& get_teardown_func_map__();
std::function<void()>&                                  get_global_setup__();
std::function<void()>&                                  get_global_teardown__();
std::mutex&                                             get_test_output_mu__();

void test_base__(const std::string& file, int line, const std::string& func_name, std::function<void(test_data_t__&)> func, std::function<void()> before = nullptr, std::function<void()> after = nullptr);
template <typename U>
void test_base__(const std::string& file, int line, const std::string& func_name, std::function<void(test_data_t__&, const U&)> func, const std::vector<U>& data, std::function<void()> before = nullptr, std::function<void()> after = nullptr);
void test_base__(const std::string& file, int line, const std::string& func_name, std::function<void(test_data_t__&)> func, const std::string& class_name, std::function<void()> before = nullptr, std::function<void()> after = nullptr);
template <typename U>
void test_base__(const std::string& file, int line, const std::string& func_name, std::function<void(test_data_t__&, const U&)> func, const std::string& class_name, const std::vector<U>& data, std::function<void()> before = nullptr, std::function<void()> after = nullptr);

int  run_test(int thread_count = 1, bool flashing = true);
void test_set_env(const std::string& class_name, std::function<void()> setup, std::function<void()> teardown);
void test_set_global_env(std::function<void()> setup, std::function<void()> teardown);

}

#define sf_test_env(...) skyfire::test_base__::set_env(__VA_ARGS__)
#define sf_test_global_env(...) skyfire::test_base__::set_global_env(__VA_ARGS__)

#define MAKE_TEST_VAR_NAME_WRAP(a, b) a##b
#define MAKE_TEST_VAR_NAME(a, b) MAKE_TEST_VAR_NAME_WRAP(a, b)

#define sf_test(cls, name, ...)                                                                               \
    void                     name(skyfire::test_data_t__&);                                                   \
    skyfire::object_meta_run MAKE_TEST_VAR_NAME(__SF_TEST_VAR_, __LINE__)(                                    \
        []() { skyfire::test_base__(__FILE__,                                                                 \
                                    __LINE__,                                                                 \
                                    #name,                                                                    \
                                    std::function(name),                                                      \
                                    #cls,                                                                     \
                                    ##__VA_ARGS__); }); \
    void name(skyfire::test_data_t__& __result__)

#define sf_test_p(cls, name, type, ...)                                                                       \
    void                     name(skyfire::test_data_t__&, const type&);                                      \
    skyfire::object_meta_run MAKE_TEST_VAR_NAME(__SF_TEST_VAR_, __LINE__)(                                    \
        []() { skyfire::test_base__(__FILE__,                                                                 \
                                    __LINE__,                                                                 \
                                    #name,                                                                    \
                                    std::function(name),                                                      \
                                    #cls,                                                                     \
                                    ##__VA_ARGS__); }); \
    void name(skyfire::test_data_t__& __result__, const type& test_param)

#define test_assert(exp)                                                                                 \
    if (!(exp))                                                                                          \
    {                                                                                                    \
        std::lock_guard<std::mutex> lck(skyfire::get_test_output_mu__());                                \
        std::ostringstream          so;                                                                  \
        so << __FILE__ << ":" << __LINE__ << " " << __FUNCTION__ << " : `" << #exp << "` return false!"; \
        std::cerr << skyfire::color_string(so.str(), { skyfire::color_fg_red }) << std::endl;            \
        __result__.pass = false;                                                                         \
    }

#define test_p_eq(a, b)                                                                                   \
    {                                                                                                     \
        auto __a__ = (a);                                                                                 \
        auto __b__ = (b);                                                                                 \
        if (__a__ != __b__)                                                                               \
        {                                                                                                 \
            std::lock_guard<std::mutex> lck(skyfire::get_test_output_mu__());                             \
            std::ostringstream          so;                                                               \
            so << __FILE__ << ":" << __LINE__ << " " << __FUNCTION__ << ": " << __a__ << " != " << __b__; \
            std::cerr << skyfire::color_string(so.str(), { skyfire::color_fg_red }) << std::endl;         \
            __result__.pass = false;                                                                      \
        }                                                                                                 \
    }

#define test_p_neq(a, b)                                                                                  \
    {                                                                                                     \
        auto __a__ = (a);                                                                                 \
        auto __b__ = (b);                                                                                 \
        if (__a__ == __b__)                                                                               \
        {                                                                                                 \
            std::lock_guard<std::mutex> lck(skyfire::get_test_output_mu__());                             \
            std::ostringstream          so;                                                               \
            so << __FILE__ << ":" << __LINE__ << " " << __FUNCTION__ << ": " << __a__ << " == " << __b__; \
            std::cerr << skyfire::color_string(so.str(), { skyfire::color_fg_red }) << std::endl;         \
            __result__.pass = false;                                                                      \
        }                                                                                                 \
    }

#define test_np_eq(a, b)                                                                          \
    {                                                                                             \
        if ((a) != (b))                                                                           \
        {                                                                                         \
            std::lock_guard<std::mutex> lck(skyfire::get_test_output_mu__());                     \
            std::ostringstream          so;                                                       \
            so << __FILE__ << ":" << __LINE__ << " " << __FUNCTION__ << ": `" #a "` != `" #b "`"; \
            std::cerr << skyfire::color_string(so.str(), { skyfire::color_fg_red }) << std::endl; \
            __result__.pass = false;                                                              \
        }                                                                                         \
    }

#define test_np_neq(a, b)                                                                                      \
    {                                                                                                          \
        if ((a) == (b))                                                                                        \
        {                                                                                                      \
            std::lock_guard<std::mutex> lck(get_test_output_mu__());                                           \
            std::ostringstream          so;                                                                    \
            so << __FILE__ << ":" << __LINE__ << " " << __FUNCTION__ << ": `" #a "` == `" #b "`" << std::endl; \
            std::cerr << skyfire::color_string(so.str(), { skyfire::color_fg_red }) << std::endl;              \
            __result__.pass = false;                                                                           \
        }                                                                                                      \
    }

#define test_num_eq test_p_eq
#define test_str_eq test_p_eq
#define test_bool_eq test_p_eq

#define test_num_neq test_p_neq
#define test_str_neq test_p_neq
#define test_bool_neq test_p_neq
