
/**
* @version 1.0.0
* @author skyfire
* @file sf_single_instance.h
*/

/*
 * single_instance单例支持
 */
#pragma once

#include "sf_empty_class.h"
#include "sf_stdc++.h"

namespace skyfire
{
/**
 * 单例类
 * @tparam ThisClass 要单例的类
 * @tparam BaseClass 父类
 */
template <typename ThisClass, typename BaseClass = empty_class>
class single_instance : public BaseClass
{
public:
    template <typename... Args>
    static std::shared_ptr<ThisClass> instance(Args&&... args);

private:
    single_instance() = default;

    friend ThisClass;
};

} // namespace skyfire

/*
 * 侵入式单例宏实现
 */

#define SF_SINGLE_TON(ClassName)                            \
    ClassName(const ClassName&) = delete;                   \
    ClassName(ClassName&&)      = delete;                   \
    ClassName&        operator=(const ClassName&) = delete; \
    static ClassName* instance()                            \
    {                                                       \
        static ClassName instance__;                        \
        return &instance__;                                 \
    }
