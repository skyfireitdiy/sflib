#pragma once

#include "sf_co_mutex.h"
#include "sf_co_ctx.h"

namespace skyfire
{

inline void co_mutex::lock()
{
    auto    ctx  = get_co_env()->get_curr_co();
    co_ctx* null = nullptr;
    while (!owner__.compare_exchange_strong(null, ctx))
    {
        this_coroutine::yield_coroutine();
        null = nullptr;
    }
}

inline void co_mutex::unlock()
{
    auto    ctx  = get_co_env()->get_curr_co();
    co_ctx* null = nullptr;
    while (!owner__.compare_exchange_strong(ctx, null))
    {
        this_coroutine::yield_coroutine();
        ctx = get_co_env()->get_curr_co();
    }
    this_coroutine::yield_coroutine();
}

inline bool co_mutex::try_lock()
{
    auto    ctx  = get_co_env()->get_curr_co();
    co_ctx* null = nullptr;
    return owner__.compare_exchange_strong(null, ctx);
}

inline void co_shared_mutex::lock()
{
    auto ctx = get_co_env()->get_curr_co();
    while (true)
    {
        mu__.lock();
        if (writer__ != nullptr || !reader__.empty())
        {
            mu__.unlock();
            this_coroutine::yield_coroutine();
            continue;
        }
        writer__ = ctx;
        mu__.unlock();
        return;
    }
}

inline bool co_shared_mutex::try_lock()
{
    auto ctx = get_co_env()->get_curr_co();
    if (!mu__.try_lock())
    {
        return false;
    }
    if (writer__ != nullptr || !reader__.empty())
    {
        mu__.unlock();
        return false;
    }
    writer__ = ctx;
    mu__.unlock();
    return true;
}

inline void co_shared_mutex::unlock()
{
    auto ctx = get_co_env()->get_curr_co();
    while (true)
    {
        mu__.lock();
        if (writer__ == ctx)
        {
            writer__ = nullptr;
            mu__.unlock();
            return;
        }
        mu__.unlock();
        this_coroutine::yield_coroutine();
    }
    this_coroutine::yield_coroutine();
}

inline void co_shared_mutex::lock_shared()
{
    auto ctx = get_co_env()->get_curr_co();
    while (true)
    {
        mu__.lock();
        if (writer__ != nullptr || reader__.contains(ctx))
        {
            mu__.unlock();
            this_coroutine::yield_coroutine();
            continue;
        }
        reader__.insert(ctx);
        mu__.unlock();
        return;
    }
}

inline bool co_shared_mutex::try_lock_shared()
{
    auto ctx = get_co_env()->get_curr_co();
    if (!mu__.try_lock())
    {
        return false;
    }
    if (writer__ != nullptr || reader__.contains(ctx))
    {
        mu__.unlock();
        return false;
    }
    reader__.insert(ctx);
    mu__.unlock();
    return true;
}

inline void co_shared_mutex::unlock_shared()
{
    auto ctx = get_co_env()->get_curr_co();
    while (true)
    {
        mu__.lock();
        if (!reader__.contains(ctx))
        {
            mu__.unlock();
            this_coroutine::yield_coroutine();
            continue;
        }
        reader__.erase(ctx);
        mu__.unlock();
        return;
    }
    this_coroutine::yield_coroutine();
}

inline void co_recursive_mutex::lock()
{
    auto ctx = get_co_env()->get_curr_co();
    while (true)
    {
        mu__.lock();
        if (owner__ != nullptr && owner__ != ctx)
        {
            mu__.unlock();
            this_coroutine::yield_coroutine();
            continue;
        }
        owner__ = ctx;
        ++count__;
        mu__.unlock();
        return;
    }
}

inline void co_recursive_mutex::unlock()
{
    auto ctx = get_co_env()->get_curr_co();
    while (true)
    {
        mu__.lock();
        if (owner__ != ctx)
        {
            mu__.unlock();
            this_coroutine::yield_coroutine();
            continue;
        }
        --count__;
        if (count__ == 0)
        {
            owner__ = nullptr;
        }
        mu__.unlock();
        return;
    }
    this_coroutine::yield_coroutine();
}

inline bool co_recursive_mutex::try_lock()
{
    auto ctx = get_co_env()->get_curr_co();
    if (!mu__.try_lock())
    {
        return false;
    }
    if (owner__ != nullptr && owner__ != ctx)
    {
        mu__.unlock();
        return false;
    }
    owner__ = ctx;
    ++count__;
    mu__.unlock();
    return true;
}

inline void co_timed_mutex::lock()
{
    mu__.lock();
}

inline bool co_timed_mutex::try_lock()
{
    return mu__.try_lock();
}

template <typename T>
inline bool co_timed_mutex::try_lock_for(const T& t)
{
    return try_lock_until(std::chrono::system_clock::now() + t);
}

template <typename T>
inline bool co_timed_mutex::try_lock_until(const T& t)
{
    while (std::chrono::system_clock::now() < t)
    {
        if (mu__.try_lock())
        {
            return true;
        }
    }
    return false;
}

inline void co_timed_mutex::unlock()
{
    mu__.unlock();
}

inline void co_recursive_timed_mutex::lock()
{
    mu__.lock();
}

inline bool co_recursive_timed_mutex::try_lock()
{
    return mu__.try_lock();
}

template <typename T>
bool co_recursive_timed_mutex::try_lock_for(const T& t)
{
    return try_lock_until(std::chrono::system_clock::now() + t);
}

template <typename T>
inline bool co_recursive_timed_mutex::try_lock_until(const T& t)
{
    while (std::chrono::system_clock::now() < t)
    {
        if (mu__.try_lock())
        {
            return true;
        }
    }
    return false;
}

inline void co_recursive_timed_mutex::unlock()
{
    mu__.unlock();
}

inline void co_shared_timed_mutex::lock()
{
    mu__.lock();
}

inline bool co_shared_timed_mutex::try_lock()
{
    return mu__.try_lock();
}

template <typename T>
inline bool co_shared_timed_mutex::try_lock_for(const T& t)
{
    return try_lock_until(std::chrono::system_clock::now() + t);
}

template <typename T>
inline bool co_shared_timed_mutex::try_lock_until(const T& t)
{
    while (std::chrono::system_clock::now() < t)
    {
        if (mu__.try_lock())
        {
            return true;
        }
    }
    return false;
}

inline void co_shared_timed_mutex::unlock()
{
    mu__.unlock();
}

inline void co_shared_timed_mutex::lock_shared()
{
    mu__.lock_shared();
}

inline bool co_shared_timed_mutex::try_lock_shared()
{
    return mu__.try_lock_shared();
}

template <typename T>
bool co_shared_timed_mutex::try_lock_shared_for(const T& t)
{
    return try_lock_shared_until(std::chrono::system_clock::now() + t);
}

template <typename T>
bool co_shared_timed_mutex::try_lock_shared_until(const T& t)
{
    while (std::chrono::system_clock::now() < t)
    {
        if (mu__.try_lock_shared())
        {
            return true;
        }
    }
    return false;
}

inline void co_shared_timed_mutex::unlock_shared()
{
    mu__.unlock_shared();
}

}