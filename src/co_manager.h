#pragma once

#include "co_env.h"

namespace skyfire
{
class co_manager final
{
    std::unordered_set<co_env*> co_env_set__;
    mutable std::mutex          mu_co_env_set__;
    size_t                      base_co_thread_count__ = 1;
    //std::thread::hardware_concurrency() * 2;
    bool              need_exit__ = false;
    std::future<void> monitor_future__;

    void monitor_thread__();
    void reassign_co__();

public:
    co_manager();
    std::unordered_set<co_env*> get_co_env_set() const;

    co_env* add_env();
    co_env* get_best_env();
    void    remove_env(co_env* env);
    bool    need_destroy_co_thread() const;
    ~co_manager();
};

co_manager* get_co_manager();

}