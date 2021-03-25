#pragma once

#include "co_env.h"
#include "co_manager.h"
#include "co_utils.h"
#include "coroutine.h"
#include "error.h"
namespace skyfire
{

inline std::once_flag& get_co_once_flag()
{
    static std::once_flag flag;
    return flag;
}

template <typename Func, typename... Args>
coroutine<Func, Args...>::coroutine(Func&& func, Args&&... args) requires ReturnVoid<Func, Args...>
{
    ctx__ = get_co_manager()->get_best_env()->create_coroutine(attr__, [&func, &args...]() {
        std::forward<Func>(func)(std::forward<Args>(args)...);
    });
}

template <typename Func, typename... Args>
coroutine<Func, Args...>::coroutine(Func&& func, Args&&... args) requires ReturnNotVoid<Func, Args...>
{
    ctx__ = get_co_manager()->get_best_env()->create_coroutine(attr__, [this, &func, &args...]() {
        result__ = std::forward<Func>(func)(std::forward<Args>(args)...);
    });
}

template <typename Func, typename... Args>
coroutine<Func, Args...>::coroutine(const std::initializer_list<co_attr_option_type>& attr_config, Func&& func, Args&&... args) requires ReturnNotVoid<Func, Args...>
{
    for (auto& f : attr_config)
    {
        f(attr__);
    }
    ctx__ = get_co_manager()->get_best_env()->create_coroutine(attr__, [this, &func, &args...]() {
        result__ = std::forward<Func>(func)(std::forward<Args>(args)...);
    });
}

template <typename Func, typename... Args>
coroutine<Func, Args...>::coroutine(const std::initializer_list<co_attr_option_type>& attr_config, Func&& func, Args&&... args) requires ReturnVoid<Func, Args...>
{
    for (auto& f : attr_config)
    {
        f(attr__);
    }
    ctx__ = get_co_manager()->get_best_env()->create_coroutine(attr__, [&func, &args...]() {
        std::forward<Func>(func)(std::forward<Args>(args)...);
    });
}

template <typename Func, typename... Args>
inline void coroutine<Func, Args...>::wait()
{
    while (ctx__->get_state() != co_state::finished)
    {
        this_coroutine::yield_coroutine();
    }
}

template <typename Func, typename... Args>
inline bool coroutine<Func, Args...>::joinable() const
{
    return !(joined__ || detached__);
}

template <typename Func, typename... Args>
template <typename T>
bool coroutine<Func, Args...>::wait_for(const T& t)
{
    return wait_until(std::chrono::system_clock::now() + t);
}

template <typename Func, typename... Args>
template <typename T>
bool coroutine<Func, Args...>::wait_until(const T& expire)
{
    while (ctx__->get_state() != co_state::finished)
    {
        if (std::chrono::system_clock::now() > expire)
        {
            return false;
        }
        this_coroutine::yield_coroutine();
    }
    return true;
}

template <typename Func, typename... Args>
inline void coroutine<Func, Args...>::join()
{
    if (detached__)
    {
        throw std::runtime_error("already detached");
    }
    if (joined__)
    {
        throw std::runtime_error("already joined");
    }
    joined__ = true;
    wait();
    delete ctx__;
    invalid__ = true;
}

template <typename Func, typename... Args>
inline void coroutine<Func, Args...>::detach()
{
    if (detached__)
    {
        throw std::runtime_error("already detached");
    }
    if (joined__)
    {
        throw std::runtime_error("already joined");
    }
    detached__ = true;
    invalid__  = true;
    if (ctx__->get_state() == co_state::finished)
    {
        delete ctx__;
    }
    else
    {
        ctx__->set_detached();
    }
}

template <typename Func, typename... Args>
inline bool coroutine<Func, Args...>::valid() const
{
    return !invalid__;
}

template <typename Func, typename... Args>
inline coroutine<Func, Args...>::~coroutine()
{
    if (ctx__ == nullptr)
    {
        return;
    }
    if (!joined__ && !detached__)
    {
        detach();
    }
}

template <typename Func, typename... Args>
inline coroutine<Func, Args...>::coroutine(coroutine<Func, Args...>&& other)
    : ctx__(other.ctx__)
    , detached__(other.detached__)
    , joined__(other.joined__)
    , invalid__(other.invalid__)
{
    other.ctx__     = nullptr;
    other.invalid__ = true;
}

template <typename Func, typename... Args>
inline coroutine<Func, Args...>& coroutine<Func, Args...>::operator=(coroutine<Func, Args...>&& other)
{
    ctx__      = other.ctx__;
    detached__ = other.detached__;
    joined__   = other.joined__;
    invalid__  = other.invalid__;

    other.ctx__     = nullptr;
    other.invalid__ = true;

    return *this;
}

template <typename Func, typename... Args>
std::invoke_result_t<std::decay_t<Func>, std::decay_t<Args>...> coroutine<Func, Args...>::get() requires ReturnNotVoid<Func, Args...>
{
    wait();
    return std::any_cast<std::invoke_result_t<std::decay_t<Func>, std::decay_t<Args>...>>(result__);
}

template <typename Func, typename... Args>
std::invoke_result_t<std::decay_t<Func>, std::decay_t<Args>...> coroutine<Func, Args...>::get() requires ReturnVoid<Func, Args...>
{
    wait();
}

}