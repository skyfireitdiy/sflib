
/**
* @version 1.0.0
* @author skyfire
* @mail skyfireitdiy@hotmail.com
* @see http://github.com/skyfireitdiy/sflib
* @file sf_nocopy.h

* sflib第一版本发布
* 版本号1.0.0
* 发布日期：2018-10-22
*/

/*
 * sf_nocopy 禁止拷贝
 */

#pragma once

#include "core/sf_empty_class.h"

namespace skyfire {
/**
 * @brief 禁止拷贝工具类
 * @tparam _BaseClass 基类型
 */
template <typename _BaseClass = sf_empty_class>
class sf_nocopy : public _BaseClass {
   public:
    sf_nocopy(){};
    sf_nocopy(const sf_nocopy&) = delete;
    sf_nocopy(sf_nocopy&&) = delete;
    sf_nocopy& operator=(const sf_nocopy&) = delete;
    sf_nocopy& operator=(sf_nocopy&&) = delete;
};
}    // namespace skyfire
