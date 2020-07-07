
/**
* @version 1.0.0
* @author skyfire
* @file sf_finally.hpp
*/

//
// Created by skyfire on 2018/8/22 0022.
//
#pragma once

#include "sf_stdc++.h"
#include "sf_finally.h"

namespace skyfire {
inline __sf_finally__::__sf_finally__(std::function<void()> func)
    : func__(std::move(func))
{
}

inline __sf_finally__::~__sf_finally__() { func__(); }

}    // namespace skyfire