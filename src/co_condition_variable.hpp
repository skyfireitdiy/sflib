#pragma once

#include "co_condition_variable.h"

namespace skyfire
{

inline void co_condition_variable::add_wait_co__(co_ctx* ctx)
{
    std::lock_guard<co_mutex> lock(mu_waited_co__);
    waited_co__.insert(ctx);
}

inline void co_condition_variable::erase_wait_co__(co_ctx* ctx)
{
    std::lock_guard<co_mutex> lock(mu_waited_co__);
    waited_co__.erase(ctx);
}

inline void co_condition_variable::wait(std::unique_lock<co_mutex>& lck)
{
    lck.unlock();
    auto current_co = get_co_env()->get_curr_co();
    current_co->set_wait_cond();
    add_wait_co__(current_co);
    current_co->wait_cond();
    erase_wait_co__(current_co);
    lck.lock();
}

template <ReturnBool Cond>
inline void co_condition_variable::wait(std::unique_lock<co_mutex>& lck, Cond cond)
{
    lck.unlock();
    auto current_co = get_co_env()->get_curr_co();
    current_co->set_wait_cond();
    add_wait_co__(current_co);
    current_co->wait_cond(cond);
    erase_wait_co__(current_co);
    lck.lock();
}

template <typename T>
bool co_condition_variable::wait_for(std::unique_lock<co_mutex>& lck, const T& tm)
{
    return wait_until(lck, std::chrono::system_clock::now() + tm, [] { return false; });
}

template <typename T>
bool co_condition_variable::wait_until(std::unique_lock<co_mutex>& lck, const T& tm)
{
    return wait_until(lck, tm, [] { return false; });
}

template <typename T, ReturnBool Cond>
bool co_condition_variable::wait_for(std::unique_lock<co_mutex>& lck, const T& tm, Cond cond)
{
    return wait_until(lck, std::chrono::system_clock::now() + tm, cond);
}

template <typename T, ReturnBool Cond>
bool co_condition_variable::wait_until(std::unique_lock<co_mutex>& lck, const T& tm, Cond cond)
{
    lck.unlock();
    auto current_co = get_co_env()->get_curr_co();
    current_co->set_wait_cond();
    add_wait_co__(current_co);
    auto ret = current_co->wait_cond_until(tm, cond);
    erase_wait_co__(current_co);
    lck.lock();
    return ret;
}

void co_condition_variable::notify_one()
{
    std::lock_guard<co_mutex> lock(mu_waited_co__);
    if (waited_co__.empty())
    {
        return;
    }
    (*waited_co__.begin())->unset_wait_cond();
}

void co_condition_variable::notify_all()
{
    std::lock_guard<co_mutex> lock(mu_waited_co__);
    for (auto& co : waited_co__)
    {
        co->unset_wait_cond();
    }
}

}