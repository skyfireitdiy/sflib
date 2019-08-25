
/**
* @version 1.0.0
* @author skyfire
* @mail skyfireitdiy@hotmail.com
* @see http://github.com/skyfireitdiy/sflib
* @file sf_finally.hpp

* sflib第一版本发布
* 版本号1.0.0
* 发布日期：2018-10-22
*/

//
// Created by skyfire on 2018/8/22 0022.
//
#pragma once

#include "core/sf_stdc++.h"
#include "sf_finally.h"

namespace skyfire {
inline sf_finally::sf_finally(std::function<void()> func)
    : func__(std::move(func)) {}

inline sf_finally::~sf_finally() { func__(); }

}    // namespace skyfire