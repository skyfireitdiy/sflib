#pragma once
#include "core/sf_empty_class.h"
#include "core/sf_stdc++.h"
#include "tools/sf_single_instance.h"

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
    std::string function_name;
    std::function<bool()> func;
};

template <typename = sf_empty_class>
class sf_test_base__ {
protected:
    static std::vector<sf_test_func_t__> test_data__;

public:
    sf_test_base__(const std::string& func_name, std::function<bool()> func);
    static void run(int thread_count = 1, bool flashing = true);
};

class sf_test final : public sf_test_base__<> {
public:
    using sf_test_base__::sf_test_base__;
};
}

#define sf_test_func(x) skyfire::sf_test(#x, x);