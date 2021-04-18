#pragma once

#include "co_env.h"

namespace skyfire
{
class co_manager final
{
    std::unordered_map<co_env*, std::shared_ptr<std::future<void>>> co_env_set__;
    mutable std::recursive_mutex                                    mu_co_env_set__;
    size_t                                                          base_co_thread_count__ = std::thread::hardware_concurrency() * 2;
    bool                                                            need_exit__            = false;
    std::future<void>                                               monitor_future__;

    void monitor_thread__();
    void reassign_co__();

public:
    co_manager();

    co_env* add_env();
    co_env* get_best_env();
    void    remove_env(co_env* env);
    void    append_env_to_set(co_env* env, std::shared_ptr<std::future<void>> th);
    bool    need_destroy_co_thread() const;
    void    exit_env(co_env* env);
    ~co_manager();
};

co_manager* get_co_manager();

}