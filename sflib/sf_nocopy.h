
/**
* @version 1.0.0
* @author skyfire
* @file sf_nocopy.h
*/

/*
 * nocopy 禁止拷贝
 */

#pragma once

#include "sf_empty_class.h"

namespace skyfire
{
/**
 * @brief 禁止拷贝工具类
 * @tparam _BaseClass 基类型
 */
template <typename _BaseClass = empty_class>
class nocopy : public _BaseClass
{
public:
    nocopy() {};
    nocopy(const nocopy&) = delete;
    nocopy(nocopy&&)      = delete;
    nocopy& operator=(const nocopy&) = delete;
    nocopy& operator=(nocopy&&) = delete;
};
} // namespace skyfire
