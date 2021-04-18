#pragma once

#include "co_manager.h"

namespace skyfire
{

inline co_manager* get_co_manager()
{
    static co_manager manager;
    return &manager;
}

inline void co_manager::remove_current_env()
{
    std::unique_lock<std::mutex> lck(mu_env_need_clean__);
    env_need_clean__.push_back(get_co_env());
    cond_env_need_clean__.notify_one();
}

inline co_manager::~co_manager()
{
    std::lock_guard<std::recursive_mutex> lck(mu_co_env_set__);
    need_exit__ = true;

    // 唤醒清理线程，让他正常退出
    cond_env_need_clean__.notify_one();

    // exit_env 会删除co_env_set__中的元素
    auto backup = co_env_set__;
    for (auto& env : backup)
    {
        remove_env__(env.first);
    }
}

inline co_env* co_manager::add_env()
{
    // 注意，调用此函数的时候不能给mu_env_set__加锁
    std::promise<co_env*>                            env_pointer_pro;
    std::promise<std::shared_ptr<std::future<void>>> env_thread_pro;

    env_thread_pro.set_value(std::shared_ptr<std::future<void>>(new std::future<void>(std::async([&env_pointer_pro, &env_thread_pro, this]() {
        auto env = get_co_env();
        {
            append_env_to_set(env, env_thread_pro.get_future().get());
        }
        env_pointer_pro.set_value(env);
        while (!env->if_need_exit())
        {
            this_coroutine::yield_coroutine();
        }
        sf_debug("env exit");
    }))));

    auto env = env_pointer_pro.get_future().get();
    return env;
}

inline void co_manager::append_env_to_set(co_env* env, std::shared_ptr<std::future<void>> th)
{
    std::lock_guard<std::recursive_mutex> lck(mu_co_env_set__);
    co_env_set__.insert({ env, th });
}

inline co_env* co_manager::get_best_env()
{
    if (co_env_set__.size() < base_co_thread_count__)
    {
        return add_env();
    }
    co_env* best = nullptr;
    {
        std::lock_guard<std::recursive_mutex> lck(mu_co_env_set__);
        for (auto& env : co_env_set__)
        {
            if (!env.first->if_blocked() && !env.first->if_need_exit())
            {
                if (best == nullptr || (best->co_size() > env.first->co_size()))
                {
                    best = env.first;
                }
            }
        }
    }
    return best == nullptr ? add_env() : best;
}

inline void co_manager::remove_env__(co_env* env)
{
    std::lock_guard<std::recursive_mutex> lck(mu_co_env_set__);
    env->set_exit_flag();
    co_env_set__.erase(env);
}

inline void co_manager::clean_env_thread__()
{
    while (need_exit__)
    {
        std::unique_lock<std::mutex> lck(mu_env_need_clean__);
        cond_env_need_clean__.wait(lck);
        if (!need_exit__)
        {
            return;
        }
        for (auto& p : env_need_clean__)
        {
            remove_env__(p);
        }
        env_need_clean__.clear();
    }
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
        std::lock_guard<std::recursive_mutex> lck(mu_co_env_set__);
        normal_env.reserve(co_env_set__.size());
        std::for_each(co_env_set__.begin(), co_env_set__.end(), [&need_move_co, &normal_env](auto& env) {
            if (env.first->has_sched())
            {
                // 记录正常的协程
                normal_env.push_back(env.first);
            }
            else
            {
                auto blocked_co = env.first->surrender_co();
                need_move_co.insert(need_move_co.end(), blocked_co.begin(), blocked_co.end());
                env.first->set_blocked();
            }
            env.first->reset_sched();
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
    std::lock_guard<std::recursive_mutex> lck(mu_co_env_set__);
    return co_env_set__.size() > base_co_thread_count__;
}

inline co_manager::co_manager()
{
    background_task_future__.emplace_back(std::async(&co_manager::monitor_thread__, this));
    background_task_future__.emplace_back(std::async(&co_manager::clean_env_thread__, this));
}
}