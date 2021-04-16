#pragma once

#include "co_attr.h"
#include "co_ctx.h"
#include "co_mutex.h"
#include "stdc++.h"
#include <initializer_list>

namespace skyfire
{

template <typename Func, typename... Args>
class coroutine final
{
private:
    co_ctx*        ctx__;
    bool           detached__ = false;
    bool           joined__   = false;
    bool           invalid__  = false;
    co_attr_config attr__;
    std::any       result__;
    // todo 需要替换为 co_promise

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
    void manage_this_thread();

    std::invoke_result_t<std::decay_t<Func>, std::decay_t<Args>...> get() requires ReturnNotVoid<Func, Args...>;

    std::invoke_result_t<std::decay_t<Func>, std::decay_t<Args>...> get() requires ReturnVoid<Func, Args...>;

    coroutine(Func&& func, Args&&... args) requires ReturnVoid<Func, Args...>;
    coroutine(const std::initializer_list<co_attr_option_type>& attr_config, Func&& func, Args&&... args) requires ReturnVoid<Func, Args...>;

    coroutine(Func&& func, Args&&... args) requires ReturnNotVoid<Func, Args...>;
    coroutine(const std::initializer_list<co_attr_option_type>& attr_config, Func&& func, Args&&... args) requires ReturnNotVoid<Func, Args...>;

    ~coroutine();
};

template <typename Func, typename... Args>
requires ReturnVoid<Func, Args...>
coroutine(Func&&, Args&&...) -> coroutine<Func, Args...>;

template <typename Func, typename... Args>
requires ReturnVoid<Func, Args...>
coroutine(const std::initializer_list<co_attr_option_type>& attr_config, Func&&, Args&&...) -> coroutine<Func, Args...>;

template <typename Func, typename... Args>
requires ReturnNotVoid<Func, Args...>
coroutine(Func&&, Args&&...) -> coroutine<Func, Args...>;

template <typename Func, typename... Args>
requires ReturnNotVoid<Func, Args...>
coroutine(const std::initializer_list<co_attr_option_type>& attr_config, Func&&, Args&&...) -> coroutine<Func, Args...>;

}
