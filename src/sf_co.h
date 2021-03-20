#pragma once

#include "sf_stdc++.h"
#include "sf_co_mutex.h"
#include "sf_co_ctx.h"

namespace skyfire
{



class co_ctx;
class co_env;
class co_manager;

enum class co_future_status;

template <typename Func, typename... Args>
class coroutine;
template <typename T>
class co_future;
template <typename T>
class co_promise;



template <typename Function, typename... Args>
concept ReturnVoid = std::is_same_v<std::invoke_result_t<std::decay_t<Function>, std::decay_t<Args>...>, void>;

template <typename Function, typename... Args>
concept ReturnNotVoid = !std::is_same_v<std::invoke_result_t<std::decay_t<Function>, std::decay_t<Args>...>, void>;





template <typename Func, typename... Args>
class coroutine final
{
private:
    co_ctx* ctx__;
    bool    detached__ = false;
    bool    joined__   = false;
    bool    invalid__  = false;
    // todo 需要替换为 co_promise
    std::promise<std::invoke_result_t<std::decay_t<Func>, std::decay_t<Args>...>> promise__;

    coroutine(const coroutine& ctx)             = delete;
    coroutine& operator==(const coroutine& ctx) = delete;

public:
    coroutine(coroutine&& other);
    coroutine& operator=(coroutine&& other);

    template <typename T>
    bool wait_for(const T& t);
    template <typename T>
    bool wait_until(const T& expire);
    void wait();
    void join();
    void detach();
    bool valid() const;
    bool joinable() const;

    coroutine(Func&& func, Args&&... args) requires ReturnVoid<Func, Args...>;
    coroutine(const coroutine_attr& attr, Func&& func, Args&&... args) requires ReturnVoid<Func, Args...>;

    coroutine(Func&& func, Args&&... args) requires ReturnNotVoid<Func, Args...>;
    coroutine(const coroutine_attr& attr, Func&& func, Args&&... args) requires ReturnNotVoid<Func, Args...>;

    ~coroutine();
};



template <typename Func, typename... Args>
requires ReturnVoid<Func, Args...>
coroutine(Func&&, Args&&...) -> coroutine<Func, Args...>;

template <typename Func, typename... Args>
requires ReturnVoid<Func, Args...>
coroutine(const coroutine_attr&, Func&&, Args&&...) -> coroutine<Func, Args...>;

template <typename Func, typename... Args>
requires ReturnNotVoid<Func, Args...>
coroutine(Func&&, Args&&...) -> coroutine<Func, Args...>;

template <typename Func, typename... Args>
requires ReturnNotVoid<Func, Args...>
coroutine(const coroutine_attr&, Func&&, Args&&...) -> coroutine<Func, Args...>;

template <typename T>
concept VoidType = std::is_same_v<T, void>;

template <typename T>
concept NotVoidType = !std::is_same_v<T, void>;

enum class co_future_status
{
    deferred,
    ready,
    timeout
};

template <typename T>
class co_promise;

// todo 实现
template <typename T>
class co_future
{
private:
    std::unique_ptr<T> data__;

    void set_data__(const T& t);

public:
    T    get();
    bool valid();
    void wait() const;
    template <typename Tm>
    co_future_status wait_for(const Tm& tm);
    template <typename Tm>
    co_future_status wait_until(const Tm& tm);

    friend co_promise<T>;
};

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

template <typename Function, typename... Args>
co_future<std::invoke_result_t<std::decay_t<Function>, std::decay_t<Args>...>>
co_async(Function&& f, Args&&... args);

}
