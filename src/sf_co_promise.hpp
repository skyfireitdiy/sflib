#pragma once

#include "sf_co_promise.h"

namespace skyfire
{

template <typename T>
co_future<T> co_promise<T>::get_future()
{
    return future__;
}

template <typename T>
void co_promise<T>::set_value(const T& value) requires NotVoidType<T>
{
    std::lock_guard<co_mutex> lock(mutex__);
    if (has_value__)
    {
        throw exception(err_exist, "already has value");
    }
    has_value__ = true;
    future__.set_value(value);
}
}