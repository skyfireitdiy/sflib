#pragma once

#include "co_ctx.h"
#include "co_future.h"
#include "co_mutex.h"
#include "stdc++.h"

namespace skyfire
{

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

template <typename Function, typename... Args>
co_future<std::invoke_result_t<std::decay_t<Function>, std::decay_t<Args>...>>
co_async(Function&& f, Args&&... args);

}
