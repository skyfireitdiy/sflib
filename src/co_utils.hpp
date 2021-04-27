#pragma once

#include "co_utils.h"
#include "logger.h"

#define __co_memory_barrier__() __asm volatile("" :: \
                                                   : "memory")
namespace skyfire
{
inline void __switch_co__(co_ctx* curr, co_ctx* next)
{
    if (curr->get_state() == co_state::running)
    {
        curr->set_state(co_state::suspended);
    }
    next->set_state(co_state::running);
    __co_memory_barrier__();
    __swap_regs__(curr->get_reg_buf(), next->get_reg_buf());
    __co_memory_barrier__();
}

/*| 64 bit
 *|-------------------------------------------------|
 *| low  | regs[0]: r15                 |  0         |            
 *|      | regs[1]: r14                 |  8         |            
 *|      | regs[2]: r13                 |  16        |               
 *|      | regs[3]: r12                 |  24        |         
 *|      | regs[4]: rbp                 |  32        |        
 *|      | regs[5]: rip(ret)            |  40        |         
 *|      | regs[6]: rbx                 |  48        |           
 *|      | regs[7]: rsp                 |  56        |
 *|      | regs[8]: rdi                 |  64        |       
 *| high | regs[9]: fc_mxcsr|fc_x87_cw |  72/76     |          
 *|-------------------------------------------------|
 */

inline void __swap_regs__(const void*, const void*)
{
    __asm("popq %rbp\n");
    __asm("movq %r15, (%rdi)\n");
    __asm("movq %r14, 8(%rdi)\n");
    __asm("movq %r13, 16(%rdi)\n");
    __asm("movq %r12, 24(%rdi)\n");
    __asm("movq %rbp, 32(%rdi)\n");
    __asm("movq 0(%rsp), %rax\n");
    __asm("movq %rax, 40(%rdi)\n");
    __asm("movq %rbx, 48(%rdi)\n");
    __asm("movq %rsp, 56(%rdi)\n");
    __asm("movq %rdi, 64(%rdi)\n");
    __asm("movq 64(%rsi), %rdi\n");
    __asm("movq 56(%rsi), %rsp\n");
    __asm("movq 48(%rsi), %rbx\n");
    __asm("movq 40(%rsi), %rax\n");
    __asm("movq %rax, 0(%rsp)\n");
    __asm("movq 32(%rsi), %rbp\n");
    __asm("movq 24(%rsi), %r12\n");
    __asm("movq 16(%rsi), %r11\n");
    __asm("movq 8(%rsi), %r10\n");
    __asm("movq (%rsi), %r9\n");
    __asm("pushq %rbp\n");
}

inline void __co_func__(co_ctx* ctx)
{
    ctx->get_entry()();
    get_co_env()->release_curr_co();
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

template <typename T, typename... Args>
T& this_coroutine::co_local(const std::string& name, Args&&... args)
{
    return *get_co_env()->get_curr_co()->get_co_data<T>(name, std::forward<Args>(args)...);
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

inline int co_epoll_wait(int __epfd, epoll_event* __events,
                         int __maxevents, int __timeout)
{
    auto until = std::chrono::system_clock::now();
    if (__timeout >= 0)
    {
        until += std::chrono::milliseconds(__timeout);
    }
    int wait_fds = 0;
    sf_debug("start epoll_wait");
    do
    {
        if ((wait_fds = ::epoll_wait(__epfd, __events, __maxevents, 0)) == -1)
        {
            if (errno == EINTR)
            {
                this_coroutine::yield_coroutine();
                continue;
            }
            sf_debug("wait error");
            return -1;
        }
        if (wait_fds == 0)
        {
            this_coroutine::yield_coroutine();
        }
        else
        {
            return wait_fds;
        }
    } while (__timeout >= 0 ? std::chrono::system_clock::now() < until : true);
    sf_debug("timeout");
    return 0;
}

}
