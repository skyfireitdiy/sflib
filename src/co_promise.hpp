#pragma once

#include "co_promise.h"

namespace skyfire
{

template <typename T>
co_future<T> co_promise_base<T>::get_future()
{
    return future__;
}

template <typename T>
void co_promise<T>::set_value(const T& value)
{
    std::lock_guard<co_mutex> lock { co_promise_base<T>::mutex__ };
    co_promise_base<T>::future__.value__     = value;
    co_promise_base<T>::future__.has_value__ = true;
    co_promise_base<T>::future__.valid__     = true;
}

inline void co_promise<void>::set_value()
{
    std::lock_guard<co_mutex> lock { co_promise_base<void>::mutex__ };
    co_promise_base<void>::future__.has_value__ = true;
    co_promise_base<void>::future__.valid__     = true;
}

template <typename T>
void co_promise<T>::set_value_at_coroutine_exit(const T& value)
{
    std::lock_guard<co_mutex> lock { co_promise_base<T>::mutex__ };
    co_promise_base<T>::data__           = value;
    co_promise_base<T>::will_set_value__ = true;
}

inline void co_promise<void>::set_value_at_coroutine_exit()
{
    std::lock_guard<co_mutex> lock { co_promise_base<void>::mutex__ };
    co_promise_base<void>::will_set_value__ = true;
}

template <typename T>
void co_promise_base<T>::set_exception(std::exception_ptr exp)
{
    std::lock_guard<co_mutex> lock { mutex__ };
    future__.exp__     = exp;
    future__.has_exp__ = true;
}

template <typename T>
void co_promise_base<T>::set_exception_at_coroutine_exit(std::exception_ptr exp)
{
    std::lock_guard<co_mutex> lock { mutex__ };
    exp__                = exp;
    will_set_exception__ = true;
}

}