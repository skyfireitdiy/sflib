#pragma once

#include "co_future.h"
#include "co_utils.h"
namespace skyfire
{
template <typename T>
bool co_future<T>::valid() const
{
    return valid__;
}

template <typename T>
void co_future<T>::wait() const
{
    while (!has_value__)
    {
        this_coroutine::yield_coroutine();
    }
}

template <typename T>
template <typename Tm>
bool co_future<T>::wait_for(const Tm& tm)
{
    return wait_until(std::chrono::system_clock::now() + tm);
}

template <typename T>
template <typename Tm>
bool co_future<T>::wait_until(const Tm& tm)
{
    while (std::chrono::system_clock::now() < tm)
    {
        if (has_exp__)
        {
            throw exp__;
        }
        if (has_value__)
        {
            return true;
        }
        this_coroutine::yield_coroutine();
    }
    return false;
}

}