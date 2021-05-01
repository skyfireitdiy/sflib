#include "co_env.h"
#include "co_ctx.h"
#include "co_manager.h"

namespace skyfire
{

void co_env::set_curr_co(co_ctx* ctx)
{
    current_co__ = ctx;
}

void co_env::set_exit_flag()
{
    need_exit__ = true;
}

bool co_env::if_blocked() const
{
    return blocked__;
}

size_t co_env::co_size() const
{
    // std::lock_guard<std::mutex> lck_co(mu_co_set__);
    return co_set__.size();
}

bool co_env::has_sched() const
{
    return sched__;
}

void co_env::reset_sched()
{
    sched__ = false;
}

co_ctx* co_env::create_coroutine(const co_attr_config& attr, std::function<void()> func)
{
    auto new_co = new co_ctx(func, attr);
    append_co(new_co);
    return new_co;
}

void co_env::yield_coroutine()
{
    auto ctx = choose_co();

    auto curr_co = get_curr_co();
    if (curr_co == ctx)
    {
        return;
    }

    // 当前加入就绪队列
    {
        std::lock_guard<std::mutex> lck(mu_co_set__);
        co_set__.push_back(curr_co);
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

co_ctx* co_env::get_prev_co() const
{
    return prev_co__;
}

co_ctx* co_env::get_next_co() const
{
    return next_co__;
}

char* co_env::get_shared_stack() const
{
    return shared_stack__;
}

char* co_env::get_shared_stack_bp() const
{
    return shared_stack__ + default_co_stack_size;
}

void co_env::append_co(co_ctx* ctx)
{
    std::lock_guard<std::mutex> lck_co(mu_co_set__);
    co_set__.push_back(ctx);
}

co_ctx* co_env::get_curr_co() const
{
    return current_co__;
}

void co_env::release_curr_co()
{
    auto next = choose_co();
    current_co__->set_state(co_state::finished);
    auto curr    = current_co__;
    current_co__ = next;
    __switch_co__(curr, next);
}

co_ctx* co_env::choose_co()
{
    std::lock_guard<std::mutex> lck_co(mu_co_set__);
    if (co_set__.empty())
    {
        if (get_co_manager()->need_destroy_co_thread() && used__)
        {
            get_co_manager()->remove_current_env();
        }
        return main_co__;
    }
    used__    = true;
    blocked__ = false;
    sched__   = true;
    auto ret  = co_set__.front();
    co_set__.pop_front();
    return ret;
}

co_env::co_env()
{
    shared_stack__ = new char[default_co_stack_size];
    current_co__   = new co_ctx(nullptr, co_attr_config { 0, false, "__main__" });
    main_co__      = current_co__;
    save_co__      = new co_ctx(__co_save_stack__, co_attr_config { default_co_stack_size, false, "__co_save__" });
    current_co__->set_state(co_state::running);
}

co_env::~co_env()
{
    while (true)
    {
        std::unique_lock<std::mutex> lck(mu_co_set__);
        if (co_set__.size() > 1)
        {
            lck.unlock();
            get_co_env()->yield_coroutine();
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

void co_env::set_blocked()
{
    blocked__ = true;
}

std::deque<co_ctx*> co_env::surrender_co()
{
    std::lock_guard<std::mutex> lck(mu_co_set__);
    return std::move(co_set__);
}

bool co_env::if_need_exit() const
{
    return need_exit__;
}

co_env* get_co_env()
{
    thread_local static co_env* env = new co_env;
    return env;
}

}