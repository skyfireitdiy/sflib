#pragma once

#include "co_future.h"
#include "co_mutex.h"
#include "co_utils.h"

namespace skyfire
{

template <typename T>
class co_promise_base
{
protected:
    std::any           data__;
    std::exception_ptr exp__;
    bool               will_set_value__     = false;
    bool               will_set_exception__ = false;
    co_future<T>       future__;
    co_mutex           mutex__;

public:
    co_future<T> get_future();
    void         set_exception(std::exception_ptr p);
    void         set_exception_at_coroutine_exit(std::exception_ptr p);
};

template <typename T>
class co_promise : public co_promise_base<T>
{

public:
    void set_value(const T& value);
    void set_value_at_coroutine_exit(const T& value);
};

template <>
class co_promise<void> : public co_promise_base<void>
{
public:
    void set_value();
    void set_value_at_coroutine_exit();
};

}