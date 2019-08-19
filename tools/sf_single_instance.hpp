
/**
* @version 1.0.0
* @author skyfire
* @mail skyfireitdiy@hotmail.com
* @see http://github.com/skyfireitdiy/sflib
* @file sf_single_instance.hpp

* sflib第一版本发布
* 版本号1.0.0
* 发布日期：2018-10-22
*/

/*
 * sf_single_instance单例支持
 */

#pragma once
#include "core/sf_stdc++.h"
#include "sf_single_instance.h"

namespace skyfire {

template <typename ThisClass, typename BaseClass>
template <typename... Args>
std::shared_ptr<ThisClass> sf_single_instance<ThisClass, BaseClass>::instance(
    Args &&... args) {
    if (instance__ == nullptr) {
        std::lock_guard<std::mutex> lck(mu);
        if (instance__ == nullptr) {
            instance__ = std::shared_ptr<ThisClass>(
                new ThisClass(std::forward<Args>(args)...));
        }
    }
    return instance__;
}
}    // namespace skyfire
