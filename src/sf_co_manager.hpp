#pragma once

#include "sf_co_manager.h"

namespace skyfire
{

inline co_manager* get_co_manager()
{
    static co_manager manager;
    return &manager;
}

inline co_manager::~co_manager()
{
    std::lock_guard<std::mutex> lck(mu_co_env_set__);
    need_exit__ = true;
    for (auto& env : co_env_set__)
    {
        env->set_exit_flag();
    }
    co_env_set__.clear();
}

inline std::unordered_set<co_env*> co_manager::get_co_env_set() const
{
    std::lock_guard<std::mutex> lck(mu_co_env_set__);
    return co_env_set__;
}

inline co_env* co_manager::add_env()
{
    std::promise<co_env*> pro;
    std::thread([&pro, this]() {
        auto env = get_co_env();
        {
            std::lock_guard<std::mutex> lck(mu_co_env_set__);
            co_env_set__.insert(env);
        }
        pro.set_value(env);
        while (!env->if_need_exit())
        {
            this_coroutine::yield_coroutine();
        }
        get_co_manager()->remove_env(env);
    }).detach();

    auto env = pro.get_future().get();
    return env;
}

inline co_env* co_manager::get_best_env()
{
    if (co_env_set__.size() < base_co_thread_count__)
    {
        return add_env();
    }
    co_env* best = nullptr;
    {
        std::lock_guard<std::mutex> lck(mu_co_env_set__);
        for (auto& env : co_env_set__)
        {
            if (!env->if_blocked() && !env->if_need_exit())
            {
                if (best == nullptr || (best->co_size() > env->co_size()))
                {
                    best = env;
                }
            }
        }
    }
    return best == nullptr ? add_env() : best;
}

inline void co_manager::remove_env(co_env* env)
{
    std::lock_guard<std::mutex> lck(mu_co_env_set__);
    co_env_set__.erase(env);
}

inline void co_manager::monitor_thread__()
{
    while (!need_exit__)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        if (need_exit__)
        {
            break;
        }
        reassign_co__();
    }
}

inline void co_manager::reassign_co__()
{

    std::list<co_ctx*>   need_move_co;
    std::vector<co_env*> normal_env;

    {
        std::lock_guard<std::mutex> lck(mu_co_env_set__);
        normal_env.reserve(co_env_set__.size());
        std::for_each(co_env_set__.begin(), co_env_set__.end(), [&need_move_co, &normal_env](auto& env) {
            if (env->has_sched())
            {
                // 记录正常的协程
                normal_env.push_back(env);
            }
            else
            {
                auto blocked_co = env->surrender_co();
                need_move_co.insert(need_move_co.end(), blocked_co.begin(), blocked_co.end());
                env->set_blocked();
            }
            env->reset_sched();
        });
    }

    if (need_move_co.empty())
    {
        return;
    }

    if (normal_env.empty())
    {
        auto new_env = add_env();
        new_env->append_co(need_move_co.begin(), need_move_co.end());
        return;
    }

    size_t index = 0;
    for (auto iter = need_move_co.begin(); iter != need_move_co.end(); ++iter)
    {
        normal_env[index]->append_co(*iter);
        index = (index + 1) % normal_env.size();
    }
}

inline bool co_manager::need_destroy_co_thread() const
{
    return co_env_set__.size() > base_co_thread_count__;
}

inline co_manager::co_manager()
{
    monitor_future__ = std::async(&co_manager::monitor_thread__, this);
}

}