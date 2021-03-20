#pragma once

#include "sf_co_utils.h"

namespace skyfire
{
inline void __switch_co__(co_ctx* curr, co_ctx* next)
{
    if (curr->get_state() == co_state::running)
    {
        curr->set_state(co_state::suspended);
    }
    next->set_state(co_state::running);
    __swap_regs__(curr->get_reg_buf(), next->get_reg_buf());
}

inline void __swap_regs__(const void*, const void*)
{
    __asm(
        "popq %rbp\n\t"
        "leaq (%rsp),%rax\n\t"
        "movq %rax, 104(%rdi)\n\t"
        "movq %rbx, 96(%rdi)\n\t"
        "movq %rcx, 88(%rdi)\n\t"
        "movq %rdx, 80(%rdi)\n\t"
        "movq 0(%rax), %rax\n\t"
        "movq %rax, 72(%rdi) \n\t"
        "movq %rsi, 64(%rdi)\n\t"
        "movq %rdi, 56(%rdi)\n\t"
        "movq %rbp, 48(%rdi)\n\t"
        "movq %r8, 40(%rdi)\n\t"
        "movq %r9, 32(%rdi)\n\t"
        "movq %r12, 24(%rdi)\n\t"
        "movq %r13, 16(%rdi)\n\t"
        "movq %r14, 8(%rdi)\n\t"
        "movq %r15, (%rdi)\n\t"
        "xorq %rax, %rax\n\t"
        "movq 48(%rsi), %rbp\n\t"
        "movq 104(%rsi), %rsp\n\t"
        "movq (%rsi), %r15\n\t"
        "movq 8(%rsi), %r14\n\t"
        "movq 16(%rsi), %r13\n\t"
        "movq 24(%rsi), %r12\n\t"
        "movq 32(%rsi), %r9\n\t"
        "movq 40(%rsi), %r8\n\t"
        "movq 56(%rsi), %rdi\n\t"
        "movq 80(%rsi), %rdx\n\t"
        "movq 88(%rsi), %rcx\n\t"
        "movq 96(%rsi), %rbx\n\t"
        "leaq 8(%rsp), %rsp\n\t"
        "pushq 72(%rsi)\n\t"
        "movq 64(%rsi), %rsi\n\t"
        "ret\n\t");
}

inline  void __co_func__(co_ctx* ctx)
{
    ctx->get_entry()();
    get_co_env()->release_curr_co();
    if (ctx->detached())
    {
        delete ctx;
    }
    this_coroutine::yield_coroutine();
}

inline void __co_save_stack__()
{
    for (;;)
    {
        auto prev = get_co_env()->get_prev_co();
        if (prev->shared_stack())
        {
            prev->save_stack();
        }
        auto next = get_co_env()->get_next_co();
        if (next->shared_stack())
        {
            next->restore_stack();
        }
        auto curr = get_co_env()->get_curr_co();
        get_co_env()->set_curr_co(next);
        __switch_co__(curr, next);
    }
}



inline std::string this_coroutine::get_name()
{
    return get_co_env()->get_curr_co()->get_name();
}


inline void this_coroutine::yield_coroutine()
{
    get_co_env()->yield_coroutine();
}

inline long long this_coroutine::get_id()
{
    return get_co_env()->get_curr_co()->get_id();
}


template <typename T>
void this_coroutine::sleep_for(const T& t)
{
    this_coroutine::sleep_until(std::chrono::system_clock::now() + t);
}

template <typename T>
void this_coroutine::sleep_until(const T& t)
{
    while (std::chrono::system_clock::now() < t)
    {
        this_coroutine::yield_coroutine();
    }
}

}