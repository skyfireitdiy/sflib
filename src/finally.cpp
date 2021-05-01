
//
// Created by skyfire on 2018/8/22 0022.
//
#include "finally.h"
#include <bits/stdc++.h>
namespace skyfire
{
__finally__::__finally__(std::function<void()> func)
    : func__(std::move(func))
{
}
__finally__::~__finally__() { func__(); }
} // namespace skyfire