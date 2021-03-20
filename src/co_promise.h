#pragma once

#include "co_future.h"

namespace skyfire
{

// todo 实现
template <typename T>
class co_promise
{
private:
    std::any     data__;
    bool         has_value__;
    co_future<T> future__;
    co_mutex     mutex__;

public:
    co_future<T> get_future();
    void         set_value(const T& value) requires NotVoidType<T>;
    void         set_value(T& value) requires NotVoidType<T>;
    void         set_value() requires VoidType<T>;
    void         set_value_at_coroutine_exit(const T& value) requires NotVoidType<T>;
    void         set_value_at_coroutine_exit(T& value) requires NotVoidType<T>;
    void         set_value_at_coroutine_exit() requires VoidType<T>;
    void         set_exception(std::exception_ptr p);
    void         set_exception_at_coroutine_exit(std::exception_ptr p);
};

}