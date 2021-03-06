#pragma once

#include "co_attr.h"
#include "co_ctx.h"

namespace skyfire
{

class co_env final
{
private:
    std::deque<co_ctx*> co_set__;
    mutable std::mutex  mu_co_set__;
    co_ctx*             current_co__ = nullptr;
    co_ctx*             main_co__    = nullptr;
    std::atomic<bool>   sched__ { true };
    std::atomic<bool>   need_exit__ { false };
    std::atomic<bool>   blocked__ { false };
    std::atomic<bool>   used__ { false };
    char*               shared_stack__;
    co_ctx*             next_co__;
    co_ctx*             prev_co__;
    co_ctx*             save_co__;

public:
    co_env();
    ~co_env();
    void append_co(co_ctx* ctx);
    template <typename T>
    void                 append_co(T begin, T end);
    co_ctx*              choose_co();
    void                 release_curr_co();
    co_ctx*              get_curr_co() const;
    void                 set_curr_co(co_ctx* ctx);
    co_ctx*              create_coroutine(const co_attr_config& attr, std::function<void()> func);
    void                 yield_coroutine();
    bool                 if_need_exit() const;
    void                 set_exit_flag();
    bool                 if_blocked() const;
    void                 set_blocked();
    size_t               co_size() const;
    bool                 has_sched() const;
    void                 reset_sched();
    std::deque<co_ctx*>  surrender_co();
    char*                get_shared_stack() const;
    char*                get_shared_stack_bp() const;
    co_ctx*              get_prev_co() const;
    co_ctx*              get_next_co() const;
    std::vector<co_ctx*> detached_co();
};

co_env* get_co_env();

}