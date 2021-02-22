#pragma once

#include "sf_co.h"

namespace skyfire
{

//-------------
// 64 bit
// low | regs[0]: r15 |
//    | regs[1]: r14 |
//    | regs[2]: r13 |
//    | regs[3]: r12 |
//    | regs[4]: r9  |
//    | regs[5]: r8  |
//    | regs[6]: rbp |
//    | regs[7]: rdi |
//    | regs[8]: rsi |
//    | regs[9]: ret |  //ret func addr
//    | regs[10]: rdx |
//    | regs[11]: rcx |
//    | regs[12]: rbx |
// hig | regs[13]: rsp |

constexpr int co_retindex = 9;
constexpr int co_rspindex = 13;
constexpr int co_rdiindex = 7;

void static __co_func__(co_ctx*);

inline co_manager& get_co_manager()
{
    static co_manager manager;
    return manager;
}

std::unordered_set<pthread_t> co_manager::get_co_thread_ids() const
{
    std::unique_lock<std::mutex> lck(mu_co_thread_id_set__);
    return co_thread_id__;
}

void co_manager::add_thread_id(pthread_t th)
{
    std::unique_lock<std::mutex> lck(mu_co_thread_id_set__);
    co_thread_id__.insert(th);
}

void co_manager::remove_thread_id(pthread_t th)
{
    std::unique_lock<std::mutex> lck(mu_co_thread_id_set__);
    co_thread_id__.erase(th);
}

inline co_env& get_co_env()
{
    thread_local static co_env g_env;
    return g_env;
}

inline std::once_flag& get_co_once_flag()
{
    static std::once_flag flag;
    return flag;
}

inline void co_env::create_coroutine(std::function<void()> func)
{
    auto new_co   = std::make_shared<co_ctx>();
    new_co->entry = func;
    append_co__(new_co);
}

inline std::function<void()> co_ctx::get_entry() const
{
    return entry;
}

template <typename Func, typename... Args>
inline void create_coroutine(Func func, Args&&... args)
{
    get_co_env().create_coroutine(std::bind(func, std::forward<Args>(args)...));
}

inline std::shared_ptr<co_ctx> get_current_coroutine()
{
    return get_co_env().get_current_coroutine();
}

inline void co_ctx_swap(void*, void*)
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

inline void co_env::yield_coroutine()
{
    auto ctx = choose_co();

    if (!ctx->running)
    {
        ctx->regs[co_retindex] = reinterpret_cast<void*>(&__co_func__);
        ctx->regs[co_rdiindex] = reinterpret_cast<void*>(ctx.get());
        void* sp               = ctx->stack_data.data() + ctx->stack_size - sizeof(void*);
        sp                     = (char*)((unsigned long)sp & -16LL);
        void** ret_addr        = (void**)(sp);
        *ret_addr              = reinterpret_cast<void*>(&__co_func__);
        ctx->regs[co_rspindex] = reinterpret_cast<char*>(sp) - sizeof(void*) * 2;
        ctx->running           = true;
    }
    auto curr_co = get_current_coroutine();
    if (curr_co == ctx)
    {
        return;
    }
    current_co__ = ctx;
    co_ctx_swap(&curr_co->regs, &ctx->regs);
}

inline void yield_coroutine()
{
    get_co_env().yield_coroutine();
}

inline void co_env::append_co__(std::shared_ptr<co_ctx> ctx)
{
    co_set__.push_back(ctx);
}

inline static void __co_func__(co_ctx* ctx)
{
    ctx->get_entry()();
    release_curr_co();
}

inline void release_curr_co()
{
    get_co_env().release_curr_co();
    yield_coroutine();
}

inline std::shared_ptr<co_ctx> co_env::get_current_coroutine() const
{
    return current_co__;
}

inline void co_env::release_curr_co()
{
    for (auto iter = co_set__.begin(); iter != co_set__.end(); ++iter)
    {
        if (*iter == current_co__)
        {
            co_set__.erase(iter);
            current_co__->running = false;
            break;
        }
    }
}

inline std::shared_ptr<co_ctx> co_env::choose_co()
{
    curr_co_index__ = (curr_co_index__ + 1) % co_set__.size();
    return co_set__[curr_co_index__];
}

inline void __co_sche_sighandler(int)
{
    yield_coroutine();
}

inline co_env::co_env()
{
    current_co__          = std::make_shared<co_ctx>(true);
    current_co__->running = true;
    co_set__.push_back(current_co__);
    get_co_manager().add_thread_id(pthread_self());
    std::call_once(get_co_once_flag(), []() {
        signal(SIG_CO_SCHE, __co_sche_sighandler);
        std::thread([=] {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
            auto ids = get_co_manager().get_co_thread_ids();
            for (auto& id : ids)
            {
                pthread_kill(id, SIG_CO_SCHE);
            }
        }).detach();
    });
}

inline co_env::~co_env()
{
    get_co_manager().remove_thread_id(pthread_self());
}

}