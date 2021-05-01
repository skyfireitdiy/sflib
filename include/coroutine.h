#pragma once

#include "co_attr.h"
#include "co_ctx.h"
#include "co_env.h"
#include "co_manager.h"
#include "co_mutex.h"
#include <bits/stdc++.h>

namespace skyfire
{

class coroutine final
{
private:
    co_ctx*        ctx__;
    bool           joined__  = false;
    bool           invalid__ = false;
    co_attr_config attr__;
    std::any       result__;

    coroutine(const coroutine& ctx)             = delete;
    coroutine& operator==(const coroutine& ctx) = delete;

public:
    coroutine(coroutine&& other);
    coroutine& operator=(coroutine&& other);

    template <typename T>
    bool wait_for(const T& t);
    template <typename T>
    bool        wait_until(const T& expire);
    void        wait();
    void        join();
    bool        valid() const;
    bool        joinable() const;
    long long   get_id() const;
    std::string get_name() const;

    template <typename T = void>
    T get();

    template <typename Func, typename... Args>
    coroutine(Func&& func, Args&&... args);
    template <typename Func, typename... Args>
    coroutine(const std::initializer_list<co_attr_option_type>& attr_config, Func&& func, Args&&... args);

    ~coroutine();
};

namespace this_coroutine
{
    long long   get_id();
    std::string get_name();

    template <typename T>
    void sleep_for(const T& t);

    template <typename T>
    void sleep_until(const T& t);

    void yield_coroutine();

    template <typename T, typename... Args>
    T& co_local(const std::string& name, Args&&... args);
};

// impl
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

template <typename T, typename... Args>
T& this_coroutine::co_local(const std::string& name, Args&&... args)
{
    return *get_co_env()->get_curr_co()->get_co_data<T>(name, std::forward<Args>(args)...);
}

template <typename T>
void this_coroutine::sleep_for(const T& t)
{
    this_coroutine::sleep_until(std::chrono::system_clock::now() + t);
}

template <typename T>
void this_coroutine::sleep_until(const T& t)
{
    while (std::chrono::system_clock::now() < t)
    {
        this_coroutine::yield_coroutine();
    }
}

// impl

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

template <typename T>
T coroutine::get()
{
    wait();
    if constexpr (!std::is_same_v<T, void>)
    {
        return std::any_cast<T>(result__);
    }
}
}
