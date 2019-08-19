
/**
* @version 1.0.0
* @author skyfire
* @mail skyfireitdiy@hotmail.com
* @see http://github.com/skyfireitdiy/sflib
* @file sf_single_instance.h

* sflib第一版本发布
* 版本号1.0.0
* 发布日期：2018-10-22
*/

/*
 * sf_single_instance单例支持
 */
#pragma once
#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCUnusedStructInspection"

#include "core/sf_empty_class.h"
#include "core/sf_stdc++.h"

namespace skyfire {
/**
 * 单例类
 * @tparam ThisClass 要单例的类
 * @tparam BaseClass 父类
 */
template <typename ThisClass, typename BaseClass = sf_empty_class>
class sf_single_instance : public BaseClass {
   public:
    template <typename... Args>
    static std::shared_ptr<ThisClass> instance(Args&&... args);

   private:
    sf_single_instance() = default;

    inline static std::shared_ptr<ThisClass> instance__{nullptr};
    inline static std::mutex mu;
    friend ThisClass;
};

}    // namespace skyfire

/*
 * 侵入式单例宏实现
 */

#define SF_SINGLE_TON(ClassName)                         \
    ClassName(const ClassName&) = delete;                \
    ClassName(ClassName&&) = delete;                     \
    ClassName& operator=(const ClassName&) = delete;     \
    static ClassName* instance() {                       \
        static std::mutex init_mutex;                    \
        static ClassName* instance__{nullptr};           \
        if (instance__ == nullptr) {                     \
            std::lock_guard<std::mutex> lck(init_mutex); \
            if (instance__ == nullptr) {                 \
                instance__ = new ClassName;              \
            }                                            \
        }                                                \
        return instance__;                               \
    }

#pragma clang diagnostic pop