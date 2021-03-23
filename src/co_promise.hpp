#pragma once

#include "co_promise.h"

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
    future__.set_value(value);
}

template <typename T>
void co_promise<T>::set_value() requires VoidType<T>
{
    std::lock_guard<co_mutex> lock(mutex__);
    future__.set_value();
}

}