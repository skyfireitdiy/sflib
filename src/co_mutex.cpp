
#include "co_mutex.h"
#include "co_ctx.h"
#include "co_env.h"

namespace skyfire
{

void co_mutex::lock()
{
    auto    ctx  = get_co_env()->get_curr_co();
    co_ctx* null = nullptr;
    while (!owner__.compare_exchange_strong(null, ctx))
    {
        get_co_env()->yield_coroutine();
        null = nullptr;
    }
}

void co_mutex::unlock()
{
    auto    ctx  = get_co_env()->get_curr_co();
    co_ctx* null = nullptr;
    while (!owner__.compare_exchange_strong(ctx, null))
    {
        get_co_env()->yield_coroutine();
        ctx = get_co_env()->get_curr_co();
    }
    get_co_env()->yield_coroutine();
}

bool co_mutex::try_lock()
{
    auto    ctx  = get_co_env()->get_curr_co();
    co_ctx* null = nullptr;
    return owner__.compare_exchange_strong(null, ctx);
}

void co_shared_mutex::lock()
{
    auto ctx = get_co_env()->get_curr_co();
    while (true)
    {
        mu__.lock();
        if (writer__ != nullptr || !reader__.empty())
        {
            mu__.unlock();
            get_co_env()->yield_coroutine();
            continue;
        }
        writer__ = ctx;
        mu__.unlock();
        return;
    }
}

bool co_shared_mutex::try_lock()
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

void co_shared_mutex::unlock()
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
        get_co_env()->yield_coroutine();
    }
    get_co_env()->yield_coroutine();
}

void co_shared_mutex::lock_shared()
{
    auto ctx = get_co_env()->get_curr_co();
    while (true)
    {
        mu__.lock();
        if (writer__ != nullptr || reader__.contains(ctx))
        {
            mu__.unlock();
            get_co_env()->yield_coroutine();
            continue;
        }
        reader__.insert(ctx);
        mu__.unlock();
        return;
    }
}

bool co_shared_mutex::try_lock_shared()
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

void co_shared_mutex::unlock_shared()
{
    auto ctx = get_co_env()->get_curr_co();
    while (true)
    {
        mu__.lock();
        if (!reader__.contains(ctx))
        {
            mu__.unlock();
            get_co_env()->yield_coroutine();
            continue;
        }
        reader__.erase(ctx);
        mu__.unlock();
        return;
    }
    get_co_env()->yield_coroutine();
}

void co_recursive_mutex::lock()
{
    auto ctx = get_co_env()->get_curr_co();
    while (true)
    {
        mu__.lock();
        if (owner__ != nullptr && owner__ != ctx)
        {
            mu__.unlock();
            get_co_env()->yield_coroutine();
            continue;
        }
        owner__ = ctx;
        ++count__;
        mu__.unlock();
        return;
    }
}

void co_recursive_mutex::unlock()
{
    auto ctx = get_co_env()->get_curr_co();
    while (true)
    {
        mu__.lock();
        if (owner__ != ctx)
        {
            mu__.unlock();
            get_co_env()->yield_coroutine();
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
    get_co_env()->yield_coroutine();
}

bool co_recursive_mutex::try_lock()
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

void co_timed_mutex::lock()
{
    mu__.lock();
}

bool co_timed_mutex::try_lock()
{
    return mu__.try_lock();
}

void co_timed_mutex::unlock()
{
    mu__.unlock();
}

void co_recursive_timed_mutex::lock()
{
    mu__.lock();
}

bool co_recursive_timed_mutex::try_lock()
{
    return mu__.try_lock();
}

void co_recursive_timed_mutex::unlock()
{
    mu__.unlock();
}

void co_shared_timed_mutex::lock()
{
    mu__.lock();
}

bool co_shared_timed_mutex::try_lock()
{
    return mu__.try_lock();
}

void co_shared_timed_mutex::unlock()
{
    mu__.unlock();
}

void co_shared_timed_mutex::lock_shared()
{
    mu__.lock_shared();
}

bool co_shared_timed_mutex::try_lock_shared()
{
    return mu__.try_lock_shared();
}

void co_shared_timed_mutex::unlock_shared()
{
    mu__.unlock_shared();
}

}