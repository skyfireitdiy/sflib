#pragma once

#include "co_future.h"
#include "co_utils.h"
namespace skyfire
{
bool co_future_base::valid() const
{
    return valid__;
}

void co_future_base::wait() const
{
    while (!has_value__)
    {
        this_coroutine::yield_coroutine();
    }
}

template <typename Tm>
bool co_future_base::wait_for(const Tm& tm)
{
    return wait_until(std::chrono::system_clock::now() + tm);
}

template <typename Tm>
bool co_future_base::wait_until(const Tm& tm)
{
    while (std::chrono::system_clock::now() < tm)
    {
        if (has_value__)
        {
            return true;
        }
        this_coroutine::yield_coroutine();
    }
    return false;
}

}