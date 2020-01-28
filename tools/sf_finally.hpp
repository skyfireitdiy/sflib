
/**
* @version 1.0.0
* @author skyfire
* @file sf_finally.hpp
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