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

static void             __co_func__(co_ctx*);
void                    __co_sche_sighandler(int);
std::shared_ptr<co_env> get_co_env();

class co_manager
{
    std::unordered_set<std::shared_ptr<co_env>> co_env_set__;
    mutable std::mutex                          mu_co_env_set__;

    size_t base_co_thread_count__ = std::thread::hardware_concurrency() * 2;

    void monitor_thread__();

public:
    std::unordered_set<std::shared_ptr<co_env>> get_co_env_set() const;

    std::shared_ptr<co_env> add_env();
    std::shared_ptr<co_env> get_best_env();
    void                    remove_env(std::shared_ptr<co_env> env);
    bool                    need_destroy_co_thread() const;
    ~co_manager();
};

class co_ctx
{
    void*                 regs__[14] = {};
    co_state              state__    = co_state::ready;
    std::vector<char>     stack_data__;
    size_t                stack_size__;
    std::function<void()> entry__;

public:
    co_ctx(size_t ss = default_co_stack_size)
        : stack_data__(ss)
        , stack_size__(ss)
    {
    }

    std::function<void()> get_entry() const;

    co_state get_state() const;

    friend class co_env;
};

class co_env
{
private:
    std::vector<std::shared_ptr<co_ctx>> co_set__;
    std::mutex                           mu_co_set__;
    int                                  curr_co_index__ = 0;
    std::shared_ptr<co_ctx>              current_co__    = nullptr;
    std::shared_ptr<co_ctx>              main_co__       = nullptr;
    std::future<void>                    env_future__;
    std::atomic<bool>                    sched__ { false };
    std::atomic<bool>                    need_exit__ { false };
    std::atomic<bool>                    block__ { false };

public:
    co_env();
    void                                 append_co(std::shared_ptr<co_ctx> ctx);
    std::shared_ptr<co_ctx>              choose_co();
    void                                 release_curr_co();
    std::shared_ptr<co_ctx>              get_current_coroutine() const;
    std::shared_ptr<co_ctx>              create_coroutine(size_t default_stack_size, std::function<void()> func);
    void                                 yield_coroutine();
    bool                                 if_need_exit() const;
    void                                 set_exit_flag();
    void                                 set_env_future(std::future<void>&& fu);
    bool                                 if_blocked() const;
    size_t                               co_size() const;
    bool                                 has_sched() const;
    void                                 reset_sched();
    std::vector<std::shared_ptr<co_ctx>> surrender_co();
};

inline bool co_env::if_need_exit() const
{
    return need_exit__;
}

inline void co_env::set_exit_flag()
{
    need_exit__ = true;
}

inline void co_env::set_env_future(std::future<void>&& fu)
{
    env_future__ = std::move(fu);
}

inline bool co_env::if_blocked() const
{
    return block__;
}

inline size_t co_env::co_size() const
{
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

inline co_manager::~co_manager()
{
    {
        std::lock_guard<std::mutex> lck(mu_co_env_set__);
        for (auto& env : co_env_set__)
        {
            env->set_exit_flag();
        }
    }
}

inline std::shared_ptr<co_manager> get_co_manager()
{
    static std::shared_ptr<co_manager> manager = std::make_shared<co_manager>();
    return manager;
}

inline std::unordered_set<std::shared_ptr<co_env>> co_manager::get_co_env_set() const
{
    std::lock_guard<std::mutex> lck(mu_co_env_set__);
    return co_env_set__;
}

inline std::shared_ptr<co_env> co_manager::add_env()
{
    std::promise<std::shared_ptr<co_env>> pro;
    auto                                  fu = std::async([&pro, this]() {
        auto env = get_co_env();
        {
            std::lock_guard<std::mutex> lck(mu_co_env_set__);
            co_env_set__.insert(env);
        }
        pro.set_value(env);
        while (!env->if_need_exit())
        {
            yield_coroutine();
        }
        get_co_manager()->remove_env(get_co_env());
    });

    auto env = pro.get_future().get();
    env->set_env_future(std::move(fu));
    return env;
}

inline std::shared_ptr<co_env> co_manager::get_best_env()
{
    if (co_env_set__.size() < base_co_thread_count__)
    {
        return add_env();
    }
    std::shared_ptr<co_env> best = nullptr;
    {
        std::lock_guard<std::mutex> lck(mu_co_env_set__);
        for (auto& env : co_env_set__)
        {
            if (!env->if_blocked() && !env->if_blocked())
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

inline void co_manager::remove_env(std::shared_ptr<co_env> env)
{
    std::lock_guard<std::mutex> lck(mu_co_env_set__);
    co_env_set__.erase(env);
}

inline std::shared_ptr<co_env> get_co_env()
{
    thread_local static std::shared_ptr<co_env> env = std::make_shared<co_env>();
    return env;
}

inline std::once_flag& get_co_once_flag()
{
    static std::once_flag flag;
    return flag;
}

inline std::shared_ptr<co_ctx> co_env::create_coroutine(size_t default_stack_size, std::function<void()> func)
{
    auto new_co     = std::make_shared<co_ctx>(default_stack_size);
    new_co->entry__ = func;
    append_co(new_co);
    return new_co;
}

inline std::function<void()> co_ctx::get_entry() const
{
    return entry__;
}

template <typename Func, typename... Args>
inline std::shared_ptr<co_ctx> create_coroutine(Func func, Args&&... args)
{
    return get_co_manager()->get_best_env()->create_coroutine(default_co_stack_size, std::bind(func, std::forward<Args>(args)...));
}

template <typename Func, typename... Args>
inline std::shared_ptr<co_ctx> create_coroutine(size_t default_stack_size, Func func, Args&&... args)
{
    return get_co_manager()->get_best_env()->create_coroutine(default_stack_size, std::bind(func, std::forward<Args>(args)...));
}

inline std::shared_ptr<co_ctx> get_current_coroutine()
{
    return get_co_env()->get_current_coroutine();
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

inline void wait_coroutine(std::shared_ptr<co_ctx> ctx)
{
    while (ctx->get_state() != co_state::finished)
    {
        yield_coroutine();
    }
}

inline void co_env::yield_coroutine()
{
    auto ctx = choose_co();

    if (ctx->state__ == co_state::ready || ctx->state__ == co_state::finished)
    {
        ctx->regs__[co_retindex] = reinterpret_cast<void*>(&__co_func__);
        ctx->regs__[co_rdiindex] = reinterpret_cast<void*>(ctx.get());
        void* sp                 = ctx->stack_data__.data() + ctx->stack_size__ - sizeof(void*);
        sp                       = (char*)((unsigned long)sp & -16LL);
        void** ret_addr          = (void**)(sp);
        *ret_addr                = reinterpret_cast<void*>(&__co_func__);
        ctx->regs__[co_rspindex] = reinterpret_cast<char*>(sp) - sizeof(void*) * 2;
    }
    auto curr_co = get_current_coroutine();
    if (curr_co == ctx)
    {
        return;
    }
    current_co__ = ctx;
    if (curr_co->state__ == co_state::running)
    {
        curr_co->state__ = co_state::suspended;
    }
    ctx->state__ = co_state::running;
    co_ctx_swap(&curr_co->regs__, &ctx->regs__);
}

inline void yield_coroutine()
{
    get_co_env()->yield_coroutine();
}

inline void co_env::append_co(std::shared_ptr<co_ctx> ctx)
{
    std::lock_guard<std::mutex> lck_co(mu_co_set__);
    co_set__.push_back(ctx);
}

inline static void __co_func__(co_ctx* ctx)
{
    ctx->get_entry()();
    release_curr_co();
}

inline void release_curr_co()
{
    get_co_env()->release_curr_co();
    yield_coroutine();
}

inline std::shared_ptr<co_ctx> co_env::get_current_coroutine() const
{
    return current_co__;
}

inline void co_env::release_curr_co()
{
    std::lock_guard<std::mutex> lck(mu_co_set__);
    for (auto iter = co_set__.begin(); iter != co_set__.end(); ++iter)
    {
        if (*iter == current_co__)
        {
            co_set__.erase(iter);
            current_co__->state__ = co_state::finished;
            break;
        }
    }
}

inline std::shared_ptr<co_ctx> co_env::choose_co()
{
    std::lock_guard<std::mutex> lck_co(mu_co_set__);
    if (co_set__.empty())
    {
        return main_co__;
    }
    block__         = false;
    sched__         = true;
    curr_co_index__ = (curr_co_index__ + 1) % co_set__.size();
    return co_set__[curr_co_index__];
}

inline void __co_sche_sighandler(int)
{
    yield_coroutine();
}

inline co_env::co_env()
{
    current_co__          = std::make_shared<co_ctx>(0);
    main_co__             = current_co__;
    current_co__->state__ = co_state::running;
}

template <typename Tm>
bool wait_coroutine_for(std::shared_ptr<co_ctx> ctx, Tm t)
{
    auto now    = std::chrono::system_clock::now();
    auto expire = now + t;
    while (ctx->get_state() != co_state::finished)
    {
        if (std::chrono::system_clock::now() > expire)
        {
            return false;
        }
        yield_coroutine();
    }
    return true;
}

template <typename Tm>
bool wait_coroutine_until(std::shared_ptr<co_ctx> ctx, Tm expire)
{
    while (ctx->get_state() != co_state::finished)
    {
        if (std::chrono::system_clock::now() > expire)
        {
            return false;
        }
        yield_coroutine();
    }
    return true;
}

inline co_state co_ctx::get_state() const
{
    return state__;
}

inline void co_manager::monitor_thread__()
{
    while (true)
    {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        std::lock_guard<std::mutex> lck(mu_co_env_set__);

        std::vector<std::shared_ptr<co_ctx>> need_move_co;
        std::vector<std::shared_ptr<co_env>> normal_env;

        std::for_each(co_env_set__.begin(), co_env_set__.end(), [&need_move_co, &normal_env](auto& env) {
            if (!env->has_sched())
            {
                // 指定时间未发生调度的，可能死循环了，转移其他协程
                auto blocked_co = env->surrender_co();
                need_move_co.insert(need_move_co.end(), blocked_co.begin(), blocked_co.end());
            }
            else
            {
                // 记录正常的协程
                normal_env.push_back(env);
            }
            env->reset_sched();
        });
    }
}

inline std::vector<std::shared_ptr<co_ctx>> co_env::surrender_co()
{
    std::lock_guard<std::mutex> lck(mu_co_set__);
    auto                        iter = std::remove(co_set__.begin(), co_set__.end(), current_co__);
    block__                          = true;
    auto ret                         = std::vector<std::shared_ptr<co_ctx>>(iter, co_set__.end());
    co_set__.erase(iter, co_set__.end());
    return ret;
}

}