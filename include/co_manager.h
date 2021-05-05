#pragma once

#include <bits/stdc++.h>

namespace skyfire
{

class co_ctx;
class co_env;

class co_manager final
{
    std::unordered_map<co_env*, std::shared_ptr<std::future<void>>> co_env_set__;
    mutable std::mutex                                              mu_co_env_set__;
    size_t                                                          base_co_thread_count__ = std::thread::hardware_concurrency() * 2;

    std::atomic<bool> need_monitor_exit__ { false };
    std::atomic<bool> need_detach_exit__ { false };
    std::atomic<bool> need_env_exit__ { false };

    std::future<void> fu_monitor_thread__;
    std::future<void> fu_clean_env_thread__;
    std::future<void> fu_detach_thread__;

    std::deque<co_env*>     env_need_clean__;
    std::mutex              mu_env_need_clean__;
    std::condition_variable cond_env_need_clean__;

    std::deque<co_ctx*>     detached_co__;
    std::mutex              mu_detached_co__;
    std::condition_variable cond_detached_co__;

    void clean_env_thread__();
    void monitor_thread__();
    void detach_thread__();
    void reassign_co__();
    void remove_env__(co_env* env);

public:
    co_manager();

    co_env* add_env();
    co_env* get_best_env();

    void append_env_to_set(co_env* env, std::shared_ptr<std::future<void>> th);
    bool need_destroy_co_thread() const;
    void remove_current_env();
    void detached_co(co_ctx* ctx);
    ~co_manager();
};

co_manager* get_co_manager();

}