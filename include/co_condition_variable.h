#pragma once

#include "co_ctx.h"
#include "co_env.h"
#include "co_mutex.h"
#include <bits/stdc++.h>

namespace skyfire
{

class co_condition_variable
{
private:
    std::unordered_set<co_ctx*> waited_co__;
    co_mutex                    mu_waited_co__;

    void add_wait_co__(co_ctx* ctx);
    void erase_wait_co__(co_ctx* ctx);

public:
    void wait(std::unique_lock<co_mutex>& lck);
    template <ReturnBool Cond>
    void wait(std::unique_lock<co_mutex>& lck, Cond cond);
    template <typename T>
    bool wait_for(std::unique_lock<co_mutex>& lck, const T& tm);
    template <typename T, ReturnBool Cond>
    bool wait_for(std::unique_lock<co_mutex>& lck, const T& tm, Cond cond);

    template <typename T>
    bool wait_until(std::unique_lock<co_mutex>& lck, const T& expire);
    template <typename T, ReturnBool Cond>
    bool wait_until(std::unique_lock<co_mutex>& lck, const T& expire, Cond cond);

    void notify_one();
    void notify_all();
};

// impl

template <ReturnBool Cond>
void co_condition_variable::wait(std::unique_lock<co_mutex>& lck, Cond cond)
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
}