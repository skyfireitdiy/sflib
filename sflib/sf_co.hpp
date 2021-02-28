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

static void __co_func__(co_ctx*);
co_env*&    get_co_env();

class co_manager
{
    std::unordered_set<co_env*> co_env_set__;
    mutable std::mutex          mu_co_env_set__;
    std::future<void>           monitor_future__;
    size_t                      base_co_thread_count__ = 1; //std::thread::hardware_concurrency() * 2;

    void monitor_thread__();
    void reassign_co__();

public:
    co_manager();
    std::unordered_set<co_env*> get_co_env_set() const;

    co_env* add_env();
    co_env* get_best_env();
    void    remove_env(co_env* env);
    bool    need_destroy_co_thread() const;
    ~co_manager();
};

class co_ctx
{
    const void*           regs__[14] = {};
    co_state              state__    = co_state::ready;
    std::vector<char>     stack_data__;
    size_t                stack_size__;
    std::function<void()> entry__;

public:
    co_ctx(std::function<void()> entry, size_t ss = default_co_stack_size)
        : stack_data__(ss)
        , stack_size__(ss)
        , entry__(entry)
    {
    }

    std::function<void()> get_entry() const;

    co_state    get_state() const;
    void        set_state(co_state state);
    void        set_reg_ret(const void* value);
    void        set_reg_rdi(const void* value);
    void        set_reg_rsp(const void* value);
    const void* get_stack_bp() const;
    const void* get_reg_buf() const;

    // friend class co_env;
};

class co_env
{
private:
    std::vector<co_ctx*> co_set__;
    std::mutex           mu_co_set__;
    int                  curr_co_index__ = 0;
    co_ctx*              current_co__    = nullptr;
    co_ctx*              main_co__       = nullptr;
    std::future<void>    env_future__;
    std::atomic<bool>    sched__ { false };
    std::atomic<bool>    need_exit__ { false };
    std::atomic<bool>    blocked__ { false };
    std::atomic<bool>    used__ { false };

public:
    co_env();
    void append_co(co_ctx* ctx);
    template <typename T>
    void                 append_co(T begin, T end);
    co_ctx*              choose_co();
    void                 release_curr_co();
    co_ctx*              get_current_coroutine() const;
    co_ctx*              create_coroutine(size_t default_stack_size, std::function<void()> func);
    void                 yield_coroutine();
    bool                 if_need_exit() const;
    void                 set_exit_flag();
    void                 set_env_future(std::future<void>&& fu);
    bool                 if_blocked() const;
    void                 set_blocked();
    size_t               co_size() const;
    bool                 has_sched() const;
    void                 reset_sched();
    std::vector<co_ctx*> surrender_co();
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
    return blocked__;
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
    std::lock_guard<std::mutex> lck(mu_co_env_set__);
    for (auto& env : co_env_set__)
    {
        env->set_exit_flag();
    }
}

inline co_manager* get_co_manager()
{
    static co_manager* manager = new co_manager;
    return manager;
}

inline std::unordered_set<co_env*> co_manager::get_co_env_set() const
{
    std::lock_guard<std::mutex> lck(mu_co_env_set__);
    return co_env_set__;
}

inline co_env* co_manager::add_env()
{
    std::cout << "add env" << std::endl;
    std::promise<co_env*> pro;
    auto                  fu = std::async([&pro, this]() {
        auto& env = get_co_env();
        env       = new co_env();
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

inline co_env* co_manager::get_best_env()
{
    if (co_env_set__.size() < base_co_thread_count__)
    {
        std::cout << "当前线程数量：" << co_env_set__.size() << " 基准线程数量：" << base_co_thread_count__ << std::endl;
        return add_env();
    }
    co_env* best = nullptr;
    {
        std::lock_guard<std::mutex> lck(mu_co_env_set__);
        for (auto& env : co_env_set__)
        {
            std::cout << "blocked:" << env->if_blocked() << " need_exit:" << env->if_need_exit() << std::endl;
            if (!env->if_blocked() && !env->if_need_exit())
            {
                if (best == nullptr || (best->co_size() > env->co_size()))
                {
                    best = env;
                }
            }
        }
    }
    std::cout << "best:" << best << std::endl;
    return best == nullptr ? add_env() : best;
}

inline void co_manager::remove_env(co_env* env)
{
    std::lock_guard<std::mutex> lck(mu_co_env_set__);
    delete env;
    co_env_set__.erase(env);
}

inline co_env*& get_co_env()
{
    thread_local static co_env* env = nullptr;
    return env;
}

inline std::once_flag& get_co_once_flag()
{
    static std::once_flag flag;
    return flag;
}

inline co_ctx* co_env::create_coroutine(size_t default_stack_size, std::function<void()> func)
{
    auto new_co = new co_ctx(func, default_stack_size);
    append_co(new_co);
    return new_co;
}

inline std::function<void()> co_ctx::get_entry() const
{
    return entry__;
}

template <typename Func, typename... Args>
inline co_ctx* create_coroutine(Func func, Args&&... args)
{
    return get_co_manager()->get_best_env()->create_coroutine(default_co_stack_size, std::bind(func, std::forward<Args>(args)...));
}

template <typename Func, typename... Args>
inline co_ctx* create_coroutine(size_t default_stack_size, Func func, Args&&... args)
{
    return get_co_manager()->get_best_env()->create_coroutine(default_stack_size, std::bind(func, std::forward<Args>(args)...));
}

inline co_ctx* get_current_coroutine()
{
    return get_co_env()->get_current_coroutine();
}

inline void co_ctx_swap(const void*, const void*)
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

inline void wait_coroutine(co_ctx* ctx)
{
    while (ctx->get_state() != co_state::finished)
    {
        yield_coroutine();
    }
}

inline void co_env::yield_coroutine()
{
    auto ctx = choose_co();

    if (ctx->get_state() == co_state::ready || ctx->get_state() == co_state::finished)
    {
        ctx->set_reg_ret(reinterpret_cast<void*>(&__co_func__));
        ctx->set_reg_rdi(reinterpret_cast<void*>(ctx));
        const void* sp  = reinterpret_cast<const char*>(ctx->get_stack_bp()) - sizeof(void*);
        sp              = reinterpret_cast<char*>((unsigned long)sp & -16LL);
        void** ret_addr = (void**)(sp);
        *ret_addr       = reinterpret_cast<void*>(&__co_func__);
        ctx->set_reg_rsp(reinterpret_cast<const char*>(sp) - sizeof(void*) * 2);
    }
    auto curr_co = get_current_coroutine();
    if (curr_co == ctx)
    {
        return;
    }
    current_co__ = ctx;
    if (curr_co->get_state() == co_state::running)
    {
        curr_co->set_state(co_state::suspended);
    }
    ctx->set_state(co_state::running);
    co_ctx_swap(curr_co->get_reg_buf(), ctx->get_reg_buf());
}

inline void yield_coroutine()
{
    get_co_env()->yield_coroutine();
}

inline void co_env::append_co(co_ctx* ctx)
{
    std::lock_guard<std::mutex> lck_co(mu_co_set__);
    co_set__.push_back(ctx);
}

template <typename T>
void co_env::append_co(T begin, T end)
{
    std::lock_guard<std::mutex> lck_co(mu_co_set__);
    co_set__.insert(co_set__.end(), begin, end);
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

inline co_ctx* co_env::get_current_coroutine() const
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
            current_co__->set_state(co_state::finished);
            break;
        }
    }
}

inline co_ctx* co_env::choose_co()
{
    std::lock_guard<std::mutex> lck_co(mu_co_set__);
    if (co_set__.empty())
    {
        if (get_co_manager()->need_destroy_co_thread() && used__)
        {
            need_exit__ = true;
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
    current_co__ = new co_ctx(nullptr, 0);
    main_co__    = current_co__;
    current_co__->set_state(co_state::running);
}

template <typename Tm>
bool wait_coroutine_for(co_ctx* ctx, Tm t)
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
bool wait_coroutine_until(co_ctx* ctx, Tm expire)
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

inline void co_ctx::set_state(co_state state)
{
    state__ = state;
}

inline void co_ctx::set_reg_ret(const void* value)
{
    regs__[co_retindex] = value;
}

inline void co_ctx::set_reg_rdi(const void* value)
{
    regs__[co_rdiindex] = value;
}

inline void co_ctx::set_reg_rsp(const void* value)
{
    regs__[co_rspindex] = value;
}

inline const void* co_ctx::get_stack_bp() const
{
    return stack_data__.data() + stack_size__;
}

inline const void* co_ctx::get_reg_buf() const
{
    return &regs__;
}

inline void co_manager::monitor_thread__()
{
    while (true)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        reassign_co__();
    }
}

inline void co_env::set_blocked()
{
    blocked__ = true;
}

inline void co_manager::reassign_co__()
{

    std::list<co_ctx*>   need_move_co;
    std::vector<co_env*> normal_env;

    {
        std::lock_guard<std::mutex> lck(mu_co_env_set__);
        normal_env.reserve(co_env_set__.size());
        std::for_each(co_env_set__.begin(), co_env_set__.end(), [&need_move_co, &normal_env](auto& env) {
            auto blocked_co = env->surrender_co();
            need_move_co.insert(need_move_co.end(), blocked_co.begin(), blocked_co.end());
            if (env->has_sched())
            {
                // 记录正常的协程
                normal_env.push_back(env);
            }
            else
            {
                std::cout << "set blocked" << std::endl;
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

inline std::vector<co_ctx*> co_env::surrender_co()
{
    std::lock_guard<std::mutex> lck(mu_co_set__);
    auto                        iter = std::remove_if(co_set__.begin(), co_set__.end(), [this](auto& p) {
        return p != current_co__;
    });
    auto                        ret  = std::vector<co_ctx*>(iter, co_set__.end());
    co_set__.erase(iter, co_set__.end());
    return ret;
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