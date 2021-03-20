
//
// Created by skyfire on 2018/8/22 0022.
//
#pragma once
#include "sf_finally.h"
#include "sf_stdc++.h"
namespace skyfire
{
inline __finally__::__finally__(std::function<void()> func)
    : func__(std::move(func))
{
}
inline __finally__::~__finally__() { func__(); }
} // namespace skyfire