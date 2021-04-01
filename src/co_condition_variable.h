#pragma once

#include "co_ctx.h"
#include "co_mutex.h"
#include "stdc++.h"

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
    void wait(std::unique_lock<co_mutex>& lck, std::function<bool()> cond);
    template <typename T>
    bool wait_for(std::unique_lock<co_mutex>& lck, const T& tm);
    template <typename T>
    bool wait_for(std::unique_lock<co_mutex>& lck, const T& tm, std::function<bool()> cond);

    template <typename T>
    bool wait_until(std::unique_lock<co_mutex>& lck, const T& expire);
    template <typename T>
    bool wait_until(std::unique_lock<co_mutex>& lck, const T& expire, std::function<bool()> cond);

    void notify_one();
    void notify_all();
};

}