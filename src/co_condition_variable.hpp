#pragma once

#include "co_condition_variable.h"

namespace skyfire
{

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

}