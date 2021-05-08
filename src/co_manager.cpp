
#include "co_manager.h"
#include "logger.h"

namespace skyfire
{

co_manager* get_co_manager()
{
    static co_manager manager;
    return &manager;
}

void co_manager::remove_current_env()
{
    {
        std::unique_lock<std::mutex> lck(mu_env_need_clean__);
        env_need_clean__.push_back(get_co_env());
    }
    cond_env_need_clean__.notify_one();
}

co_manager::~co_manager()
{
    // 停止监控线程
    need_monitor_exit__ = true;
    fu_monitor_thread__.wait();

    // 删除所有co
    need_detach_exit__ = true;
    cond_detached_co__.notify_one();
    fu_detach_thread__.wait();

    // 删除所有环境
    auto backup = co_env_set__;
    {
        std::unique_lock<std::mutex> lck(mu_env_need_clean__);
        for (auto& env : backup)
        {
            env_need_clean__.push_back(env.first);
        }
    }
    need_env_exit__ = true;
    {
        std::lock_guard<std::mutex> lck(mu_co_env_set__);
        for (auto& env : co_env_set__)
        {
            env.first->set_exit_flag();
        }
    }
    cond_env_need_clean__.notify_one();
    fu_clean_env_thread__.wait();
}

co_env* co_manager::add_env()
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
        while (!env->if_need_exit() || env->co_size() != 0)
        {
            this_coroutine::yield_coroutine();
        }
        sf_debug("env exit");
    }))));

    auto env = env_pointer_pro.get_future().get();
    return env;
}

void co_manager::append_env_to_set(co_env* env, std::shared_ptr<std::future<void>> th)
{
    std::lock_guard<std::mutex> lck(mu_co_env_set__);
    co_env_set__.insert({ env, th });
}

co_env* co_manager::get_best_env()
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

void co_manager::remove_env__(co_env* env)
{
    std::lock_guard<std::mutex> lck(mu_co_env_set__);
    env->set_exit_flag();
    if (co_env_set__.contains(env))
    {
        co_env_set__.erase(env); // 会等待env线程退出
    }
}

void co_manager::clean_env_thread__()
{
    while (!need_env_exit__)
    {
        std::deque<co_env*> env_need_clean_backup__;
        {
            std::unique_lock<std::mutex> lck(mu_env_need_clean__);
            if (env_need_clean__.empty())
            {
                cond_env_need_clean__.wait(lck);
            }
            env_need_clean_backup__ = std::move(env_need_clean__);
        }
        for (auto& p : env_need_clean_backup__)
        {
            remove_env__(p);
        }
    }
}

void co_manager::monitor_thread__()
{
    while (!need_monitor_exit__)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        if (need_monitor_exit__)
        {
            break;
        }
        reassign_co__();
    }
}

void co_manager::reassign_co__()
{
    std::list<co_ctx*>   need_move_co;
    std::vector<co_env*> normal_env;

    {
        std::lock_guard<std::mutex> lck(mu_co_env_set__);
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

bool co_manager::need_destroy_co_thread() const
{
    // std::lock_guard<std::mutex> lck(mu_co_env_set__);
    return co_env_set__.size() > base_co_thread_count__ || need_env_exit__;
}

co_manager::co_manager()
{
    fu_monitor_thread__   = std::async(&co_manager::monitor_thread__, this);
    fu_clean_env_thread__ = std::async(&co_manager::clean_env_thread__, this);
    fu_detach_thread__    = std::async(&co_manager::detach_thread__, this);
}

void co_manager::detached_co(co_ctx* ctx)
{
    std::lock_guard<std::mutex> lck(mu_detached_co__);
    detached_co__.push_back(ctx);
    cond_detached_co__.notify_one();
}

void co_manager::detach_thread__()
{
    while (!need_detach_exit__)
    {
        std::deque<co_ctx*> detach_co_backup;
        {
            std::unique_lock<std::mutex> lck(mu_detached_co__);
            if (detached_co__.empty())
            {
                cond_detached_co__.wait(lck);
            }
            detach_co_backup = std::move(detached_co__);
        }
        for (auto& p : detach_co_backup)
        {
            while (p->get_state() != co_state::finished)
            {
                // FIXME 换用更好的办法
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
            }
            delete p;
        }
    }
}
}
