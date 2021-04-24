#pragma once

#include "co_env.h"
#include "co_manager.h"
#include "co_utils.h"

namespace skyfire
{

inline void co_env::set_curr_co(co_ctx* ctx)
{
    current_co__ = ctx;
}

inline void co_env::set_exit_flag()
{
    need_exit__ = true;
}

inline bool co_env::if_blocked() const
{
    return blocked__;
}

inline size_t co_env::co_size() const
{
    std::lock_guard<std::recursive_mutex> lck_co(mu_co_set__);
    return co_set__.size();
}

inline bool co_env::has_sched() const
{
    return sched__;
}

inline void co_env::reset_sched()
{
    sched__ = false;
}

inline co_ctx* co_env::create_coroutine(const co_attr_config& attr, std::function<void()> func)
{
    auto new_co = new co_ctx(func, attr);
    append_co(new_co);
    return new_co;
}

inline void co_env::yield_coroutine()
{
    auto ctx = choose_co();

    auto curr_co = get_curr_co();
    if (curr_co == ctx)
    {
        return;
    }

    if (curr_co->shared_stack() || ctx->shared_stack())
    {
        prev_co__    = curr_co;
        next_co__    = ctx;
        current_co__ = save_co__;
    }
    else
    {
        current_co__ = ctx;
    }
    __switch_co__(curr_co, current_co__);
}

inline co_ctx* co_env::get_prev_co() const
{
    return prev_co__;
}

inline co_ctx* co_env::get_next_co() const
{
    return next_co__;
}

inline char* co_env::get_shared_stack() const
{
    return shared_stack__;
}

inline char* co_env::get_shared_stack_bp() const
{
    return shared_stack__ + default_co_stack_size;
}

inline void co_env::append_co(co_ctx* ctx)
{
    std::lock_guard<std::recursive_mutex> lck_co(mu_co_set__);
    co_set__.push_back(ctx);
}

template <typename T>
void co_env::append_co(T begin, T end)
{
    std::lock_guard<std::recursive_mutex> lck_co(mu_co_set__);
    co_set__.insert(co_set__.end(), begin, end);
}

inline co_ctx* co_env::get_curr_co() const
{
    return current_co__;
}

inline void co_env::release_curr_co()
{
    sf_debug("release co", current_co__);
    std::lock_guard<std::recursive_mutex> lck(mu_co_set__);
    for (auto iter = co_set__.begin(); iter != co_set__.end(); ++iter)
    {
        if (*iter == current_co__)
        {
            co_set__.erase(iter);
            current_co__->set_state(co_state::finished);
            break;
        }
    }
}

inline co_ctx* co_env::choose_co()
{
    std::lock_guard<std::recursive_mutex> lck_co(mu_co_set__);
    if (co_set__.empty())
    {
        if (get_co_manager()->need_destroy_co_thread() && used__)
        {
            get_co_manager()->remove_current_env();
        }
        return main_co__;
    }
    used__          = true;
    blocked__       = false;
    sched__         = true;
    curr_co_index__ = (curr_co_index__ + 1) % co_set__.size();
    return co_set__[curr_co_index__];
}

inline co_env::co_env()
{
    shared_stack__ = new char[default_co_stack_size];
    // printf("shared stack: 0x%p  bp:0x%p\n", get_shared_stack(), get_shared_stack_bp());
    current_co__ = new co_ctx(nullptr, co_attr_config { 0, false, "__main__" });
    main_co__    = current_co__;
    // 防止主协程有业务逻辑但是无法被调度
    append_co(main_co__);
    save_co__ = new co_ctx(__co_save_stack__, co_attr_config { default_co_stack_size, false, "__co_save__" });
    current_co__->set_state(co_state::running);
}

inline co_env::~co_env()
{
    while (true)
    {
        std::unique_lock<std::recursive_mutex> lck(mu_co_set__);
        if (co_set__.size() > 1) // main_co__
        {
            lck.unlock();
            this_coroutine::yield_coroutine();
        }
        else
        {
            lck.unlock();
            break;
        }
    }
    std::cout << "co_env::~co_env" << std::endl;
    delete[] shared_stack__;
    delete save_co__;
    delete main_co__;
}

inline void co_env::set_blocked()
{
    blocked__ = true;
}

inline std::vector<co_ctx*> co_env::surrender_co()
{
    sf_debug("surrender co");
    std::lock_guard<std::recursive_mutex> lck(mu_co_set__);
    auto                                  iter = std::remove_if(co_set__.begin(), co_set__.end(), [this](auto& p) {
        return p != current_co__ && !p->shared_stack() && p != main_co__;
    });
    auto                                  ret  = std::vector<co_ctx*>(iter, co_set__.end());
    co_set__.erase(iter, co_set__.end());
    sf_debug("surrender co", ret.size(), "left", co_set__.size());
    return ret;
}

inline bool co_env::if_need_exit() const
{
    return need_exit__;
}

inline co_env* get_co_env()
{
    thread_local static co_env* env = new co_env;
    return env;
}

}