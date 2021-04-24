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
coroutine::coroutine(Func&& func, Args&&... args)
    : coroutine({}, func, args...)
{
}

template <typename Func, typename... Args>
coroutine::coroutine(const std::initializer_list<co_attr_option_type>& attr_config, Func&& func, Args&&... args)
{
    for (auto& f : attr_config)
    {
        f(attr__);
    }
    if constexpr (std::is_same_v<std::invoke_result_t<std::decay_t<Func>, std::decay_t<Args>...>, void>)
    {
        ctx__ = get_co_manager()->get_best_env()->create_coroutine(
            attr__,
            [... args = std::forward<Args>(args), func = std::forward<Func>(func)]() mutable {
                std::forward<Func>(func)(std::forward<Args>(args)...);
            });
    }
    else
    {
        ctx__ = get_co_manager()->get_best_env()->create_coroutine(
            attr__,
            [... args = std::forward<Args>(args), func = std::forward<Func>(func), this]() mutable {
                result__ = std::forward<Func>(func)(std::forward<Args>(args)...);
            });
    }
}

inline void coroutine::wait()
{
    while (ctx__->get_state() != co_state::finished)
    {
        this_coroutine::yield_coroutine();
    }
}

inline bool coroutine::joinable() const
{
    return !(joined__);
}

template <typename T>
bool coroutine::wait_for(const T& t)
{
    return wait_until(std::chrono::system_clock::now() + t);
}

template <typename T>
bool coroutine::wait_until(const T& expire)
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

inline void coroutine::join()
{
    if (joined__)
    {
        throw std::runtime_error("already joined");
    }
    joined__  = true;
    invalid__ = true;
    wait();
    delete ctx__;
}

inline bool coroutine::valid() const
{
    return !invalid__;
}

inline coroutine::~coroutine()
{
    if (ctx__ == nullptr)
    {
        return;
    }
    // if (!joined__)
    // {
    //     join();
    // }
}

inline coroutine::coroutine(coroutine&& other)
    : ctx__(other.ctx__)
    , joined__(other.joined__)
    , invalid__(other.invalid__)
{
    other.ctx__     = nullptr;
    other.invalid__ = true;
}

inline coroutine& coroutine::operator=(coroutine&& other)
{
    ctx__     = other.ctx__;
    joined__  = other.joined__;
    invalid__ = other.invalid__;

    other.ctx__     = nullptr;
    other.invalid__ = true;

    return *this;
}

template <typename T>
T coroutine::get()
{
    wait();
    if constexpr (!std::is_same_v<T, void>)
    {
        return std::any_cast<T>(result__);
    }
}

inline long long coroutine::get_id() const
{
    return ctx__->get_id();
}

inline std::string coroutine::get_name() const
{
    return ctx__->get_name();
}

}