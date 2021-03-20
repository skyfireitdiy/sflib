
//
// Created by skyfire on 2018/8/22 0022.
//
#pragma once
#include "finally.h"
#include "stdc++.h"
namespace skyfire
{
inline __finally__::__finally__(std::function<void()> func)
    : func__(std::move(func))
{
}
inline __finally__::~__finally__() { func__(); }
} // namespace skyfire