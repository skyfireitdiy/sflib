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
constexpr int co_rbpindex = 6;

static void __co_func__(co_ctx*);
static void __co_save_stack__();
static void __swap_regs__(const void*, const void*);
static void __switch_co__(co_ctx* curr, co_ctx* next);
co_env*     get_co_env();

class co_manager final
{
    std::unordered_set<co_env*> co_env_set__;
    mutable std::mutex          mu_co_env_set__;
    size_t                      base_co_thread_count__ = 1;
    //std::thread::hardware_concurrency() * 2;
    bool              need_exit__ = false;
    std::future<void> monitor_future__;

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

class co_ctx final
{
    const void*           regs__[14]   = {};
    co_state              state__      = co_state::ready;
    char*                 stack_data__ = nullptr;
    size_t                stack_size__;
    std::function<void()> entry__;
    bool                  detached__;
    mutable std::mutex    mu_detached__;
    bool                  shared_stack__;
    std::vector<char>     saved_stack__;
    std::string           name__;

public:
    co_ctx(std::function<void()> entry, const coroutine_attr& attr);

    std::function<void()> get_entry() const;
    ~co_ctx();

    co_state    get_state() const;
    void        init();
    void        set_state(co_state state);
    void        set_reg_ret(const void* value);
    void        set_reg_rdi(const void* value);
    void        set_reg_rsp(const void* value);
    void        set_reg_rbp(const void* value);
    const void* get_stack_bp() const;
    const void* get_reg_buf() const;
    void        set_detached();
    bool        detached() const;
    bool        shared_stack() const;
    void        save_stack();
    void        restore_stack();
    void        set_curr_sp(char* sp);
    std::string get_name() const;
    long long   get_id() const;
};

class co_env final
{
private:
    std::vector<co_ctx*> co_set__;
    std::mutex           mu_co_set__;
    int                  curr_co_index__ = 0;
    co_ctx*              current_co__    = nullptr;
    co_ctx*              main_co__       = nullptr;
    std::atomic<bool>    sched__ { false };
    std::atomic<bool>    need_exit__ { false };
    std::atomic<bool>    blocked__ { false };
    std::atomic<bool>    used__ { false };
    char*                shared_stack__;
    co_ctx*              next_co__;
    co_ctx*              prev_co__;
    co_ctx*              save_co__;

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
    co_ctx*              create_coroutine(const coroutine_attr& attr, std::function<void()> func);
    void                 yield_coroutine();
    bool                 if_need_exit() const;
    void                 set_exit_flag();
    bool                 if_blocked() const;
    void                 set_blocked();
    size_t               co_size() const;
    bool                 has_sched() const;
    void                 reset_sched();
    std::vector<co_ctx*> surrender_co();
    char*                get_shared_stack() const;
    char*                get_shared_stack_bp() const;
    co_ctx*              get_prev_co() const;
    co_ctx*              get_next_co() const;
};

inline bool co_env::if_need_exit() const
{
    return need_exit__;
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
    need_exit__ = true;
    for (auto& env : co_env_set__)
    {
        env->set_exit_flag();
    }
    co_env_set__.clear();
}

inline co_manager* get_co_manager()
{
    static co_manager manager;
    return &manager;
}

inline std::unordered_set<co_env*> co_manager::get_co_env_set() const
{
    std::lock_guard<std::mutex> lck(mu_co_env_set__);
    return co_env_set__;
}

inline co_env* co_manager::add_env()
{
    std::promise<co_env*> pro;
    std::thread([&pro, this]() {
        auto env = get_co_env();
        {
            std::lock_guard<std::mutex> lck(mu_co_env_set__);
            co_env_set__.insert(env);
        }
        pro.set_value(env);
        while (!env->if_need_exit())
        {
            this_coroutine::yield_coroutine();
        }
        get_co_manager()->remove_env(env);
    }).detach();

    auto env = pro.get_future().get();
    return env;
}

inline co_env* co_manager::get_best_env()
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
            if (!env->if_blocked() && !env->if_need_exit())
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

inline void co_manager::remove_env(co_env* env)
{
    std::lock_guard<std::mutex> lck(mu_co_env_set__);
    co_env_set__.erase(env);
}

inline co_env* get_co_env()
{
    thread_local static co_env env;
    return &env;
}

inline std::once_flag& get_co_once_flag()
{
    static std::once_flag flag;
    return flag;
}

inline co_ctx* co_env::create_coroutine(const coroutine_attr& attr, std::function<void()> func)
{
    auto new_co = new co_ctx(func, attr);
    append_co(new_co);
    return new_co;
}

inline std::function<void()> co_ctx::get_entry() const
{
    return entry__;
}

template <typename Func, typename... Args>
coroutine<Func, Args...>::coroutine(Func&& func, Args&&... args) requires ReturnVoid<Func, Args...>
{
    ctx__ = get_co_manager()->get_best_env()->create_coroutine(coroutine_attr {}, [this, &func, &args...]() {
        std::forward<Func>(func)(std::forward<Args>(args)...);
        promise__.set_value();
    });
}

template <typename Func, typename... Args>
coroutine<Func, Args...>::coroutine(Func&& func, Args&&... args) requires ReturnNotVoid<Func, Args...>
{
    ctx__ = get_co_manager()->get_best_env()->create_coroutine(coroutine_attr {}, [this, &func, &args...]() {
        promise__.set_value(std::forward<Func>(func)(std::forward<Args>(args)...));
    });
}

template <typename Func, typename... Args>
coroutine<Func, Args...>::coroutine(const coroutine_attr& attr, Func&& func, Args&&... args) requires ReturnNotVoid<Func, Args...>
{
    ctx__ = get_co_manager()->get_best_env()->create_coroutine(attr, [this, &func, &args...]() {
        promise__.set_value(std::forward<Func>(func)(std::forward<Args>(args)...));
    });
}

template <typename Func, typename... Args>
coroutine<Func, Args...>::coroutine(const coroutine_attr& attr, Func&& func, Args&&... args) requires ReturnVoid<Func, Args...>
{
    ctx__ = get_co_manager()->get_best_env()->create_coroutine(attr, [this, &func, &args...]() {
        std::forward<Func>(func)(std::forward<Args>(args)...);
        promise__.set_value();
    });
}

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

template <typename Func, typename... Args>
inline void coroutine<Func, Args...>::wait()
{
    while (ctx__->get_state() != co_state::finished)
    {
        this_coroutine::yield_coroutine();
    }
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

inline void co_ctx::save_stack()
{
    if (state__ != co_state::finished)
    {
        saved_stack__.assign((char*)regs__[co_rspindex], stack_data__ + stack_size__);
    }
}

inline void co_ctx::restore_stack()
{
    memcpy(stack_data__ + default_co_stack_size - saved_stack__.size(), saved_stack__.data(), saved_stack__.size());
}

inline std::string co_ctx::get_name() const
{
    return name__;
}

inline std::string this_coroutine::get_name()
{
    return get_co_env()->get_curr_co()->get_name();
}

inline co_ctx* co_env::get_prev_co() const
{
    return prev_co__;
}

inline co_ctx* co_env::get_next_co() const
{
    return next_co__;
}

inline long long co_ctx::get_id() const
{
    return reinterpret_cast<long>(this);
}

inline char* co_env::get_shared_stack() const
{
    return shared_stack__;
}

inline char* co_env::get_shared_stack_bp() const
{
    return shared_stack__ + default_co_stack_size;
}

inline void this_coroutine::yield_coroutine()
{
    get_co_env()->yield_coroutine();
}

inline long long this_coroutine::get_id()
{
    return get_co_env()->get_curr_co()->get_id();
}

template <typename Func, typename... Args>
inline bool coroutine<Func, Args...>::joinable() const
{
    return !(joined__ || detached__);
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
    get_co_env()->release_curr_co();
    if (ctx->detached())
    {
        delete ctx;
    }
    this_coroutine::yield_coroutine();
}

inline co_ctx* co_env::get_curr_co() const
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
    shared_stack__ = new char[default_co_stack_size];
    // printf("shared stack: 0x%p  bp:0x%p\n", get_shared_stack(), get_shared_stack_bp());
    current_co__ = new co_ctx(nullptr, coroutine_attr { 0, false, "__main__" });
    main_co__    = current_co__;
    save_co__    = new co_ctx(__co_save_stack__, coroutine_attr { default_co_stack_size, false, "__co_save__" });
    current_co__->set_state(co_state::running);
}

inline co_env::~co_env()
{
    delete[] shared_stack__;
}

template <typename Func, typename... Args>
template <typename T>
bool coroutine<Func, Args...>::wait_for(const T& t)
{
    return wait_until(std::chrono::system_clock::now() + t);
}

template <typename Func, typename... Args>
template <typename T>
bool coroutine<Func, Args...>::wait_until(const T& expire)
{
    while (ctx__->get_state() != co_state::finished)
    {
        if (std::chrono::system_clock::now() > expire)
        {
            return false;
        }
        this_coroutine::yield_coroutine();
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

inline void co_ctx::set_reg_rbp(const void* value)
{
    regs__[co_rbpindex] = value;
}

inline const void* co_ctx::get_stack_bp() const
{
    return stack_data__ + stack_size__;
}

inline const void* co_ctx::get_reg_buf() const
{
    return &regs__;
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
            if (env->has_sched())
            {
                // 记录正常的协程
                normal_env.push_back(env);
            }
            else
            {
                auto blocked_co = env->surrender_co();
                need_move_co.insert(need_move_co.end(), blocked_co.begin(), blocked_co.end());
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

inline bool co_ctx::shared_stack() const
{
    return shared_stack__;
}

inline std::vector<co_ctx*> co_env::surrender_co()
{
    std::lock_guard<std::mutex> lck(mu_co_set__);
    auto                        iter = std::remove_if(co_set__.begin(), co_set__.end(), [this](auto& p) {
        return p != current_co__ && !p->shared_stack();
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

template <typename Func, typename... Args>
inline void coroutine<Func, Args...>::join()
{
    if (detached__)
    {
        throw std::runtime_error("already detached");
    }
    if (joined__)
    {
        throw std::runtime_error("already joined");
    }
    joined__ = true;
    wait();
    delete ctx__;
    invalid__ = true;
}

template <typename Func, typename... Args>
inline void coroutine<Func, Args...>::detach()
{
    if (detached__)
    {
        throw std::runtime_error("already detached");
    }
    if (joined__)
    {
        throw std::runtime_error("already joined");
    }
    detached__ = true;
    invalid__  = true;
    if (ctx__->get_state() == co_state::finished)
    {
        delete ctx__;
    }
    else
    {
        ctx__->set_detached();
    }
}

template <typename Func, typename... Args>
inline bool coroutine<Func, Args...>::valid() const
{
    return !invalid__;
}

inline void co_ctx::set_detached()
{
    std::lock_guard<std::mutex> lck(mu_detached__);
    detached__ = true;
}

inline bool co_ctx::detached() const
{
    std::lock_guard<std::mutex> lck(mu_detached__);
    return detached__;
}

inline void co_ctx::init()
{
    set_reg_ret(reinterpret_cast<void*>(&__co_func__));
    set_reg_rdi(reinterpret_cast<void*>(this));
    set_reg_rbp(const_cast<void*>(get_stack_bp()));
    const void* sp  = reinterpret_cast<const char*>(get_stack_bp()) - sizeof(void*);
    sp              = reinterpret_cast<char*>((unsigned long)sp & -16LL);
    void** ret_addr = (void**)(sp);
    *ret_addr       = reinterpret_cast<void*>(&__co_func__);
    set_reg_rsp(reinterpret_cast<const char*>(sp) - sizeof(void*) * 2);
    if (shared_stack__)
    {
        save_stack();
    }
}

inline co_ctx::co_ctx(std::function<void()> entry, const coroutine_attr& attr)
    : stack_size__(attr.stack_size)
    , entry__(entry)
    , shared_stack__(attr.shared_stack)
    , name__(attr.name)
{
    if (stack_size__ != 0 && !shared_stack__)
    {
        stack_data__ = new char[stack_size__];
    }
    if (shared_stack__)
    {
        stack_size__ = default_co_stack_size;
        stack_data__ = get_co_env()->get_shared_stack();
    }
    if (stack_size__ != 0 && entry != nullptr)
    {
        init();
    }
}

template <typename Func, typename... Args>
inline coroutine<Func, Args...>::~coroutine()
{
    if (ctx__ == nullptr)
    {
        return;
    }
    if (!joined__ && !detached__)
    {
        detach();
    }
}

inline co_ctx::~co_ctx()
{
    if (stack_data__ != nullptr && !shared_stack__)
    {
        delete[] stack_data__;
    }
}

template <typename Func, typename... Args>
inline coroutine<Func, Args...>::coroutine(coroutine<Func, Args...>&& other)
    : ctx__(other.ctx__)
    , detached__(other.detached__)
    , joined__(other.joined__)
    , invalid__(other.invalid__)
{
    other.ctx__     = nullptr;
    other.invalid__ = true;
}

template <typename Func, typename... Args>
inline coroutine<Func, Args...>& coroutine<Func, Args...>::operator=(coroutine<Func, Args...>&& other)
{
    ctx__      = other.ctx__;
    detached__ = other.detached__;
    joined__   = other.joined__;
    invalid__  = other.invalid__;

    other.ctx__     = nullptr;
    other.invalid__ = true;

    return *this;
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

inline void co_mutex::lock()
{
    auto    ctx  = get_co_env()->get_curr_co();
    co_ctx* null = nullptr;
    while (!owner__.compare_exchange_strong(null, ctx))
    {
        this_coroutine::yield_coroutine();
        null = nullptr;
    }
}

inline void co_mutex::unlock()
{
    auto    ctx  = get_co_env()->get_curr_co();
    co_ctx* null = nullptr;
    while (!owner__.compare_exchange_strong(ctx, null))
    {
        this_coroutine::yield_coroutine();
        ctx = get_co_env()->get_curr_co();
    }
    this_coroutine::yield_coroutine();
}

inline bool co_mutex::try_lock()
{
    auto    ctx  = get_co_env()->get_curr_co();
    co_ctx* null = nullptr;
    return owner__.compare_exchange_strong(null, ctx);
}

inline void co_shared_mutex::lock()
{
    auto ctx = get_co_env()->get_curr_co();
    while (true)
    {
        mu__.lock();
        if (writer__ != nullptr || !reader__.empty())
        {
            mu__.unlock();
            this_coroutine::yield_coroutine();
            continue;
        }
        writer__ = ctx;
        mu__.unlock();
        return;
    }
}

inline bool co_shared_mutex::try_lock()
{
    auto ctx = get_co_env()->get_curr_co();
    if (!mu__.try_lock())
    {
        return false;
    }
    if (writer__ != nullptr || !reader__.empty())
    {
        mu__.unlock();
        return false;
    }
    writer__ = ctx;
    mu__.unlock();
    return true;
}

inline void co_shared_mutex::unlock()
{
    auto ctx = get_co_env()->get_curr_co();
    while (true)
    {
        mu__.lock();
        if (writer__ == ctx)
        {
            writer__ = nullptr;
            mu__.unlock();
            return;
        }
        mu__.unlock();
        this_coroutine::yield_coroutine();
    }
    this_coroutine::yield_coroutine();
}

inline void co_shared_mutex::lock_shared()
{
    auto ctx = get_co_env()->get_curr_co();
    while (true)
    {
        mu__.lock();
        if (writer__ != nullptr || reader__.contains(ctx))
        {
            mu__.unlock();
            this_coroutine::yield_coroutine();
            continue;
        }
        reader__.insert(ctx);
        mu__.unlock();
        return;
    }
}

inline bool co_shared_mutex::try_lock_shared()
{
    auto ctx = get_co_env()->get_curr_co();
    if (!mu__.try_lock())
    {
        return false;
    }
    if (writer__ != nullptr || reader__.contains(ctx))
    {
        mu__.unlock();
        return false;
    }
    reader__.insert(ctx);
    mu__.unlock();
    return true;
}

inline void co_shared_mutex::unlock_shared()
{
    auto ctx = get_co_env()->get_curr_co();
    while (true)
    {
        mu__.lock();
        if (!reader__.contains(ctx))
        {
            mu__.unlock();
            this_coroutine::yield_coroutine();
            continue;
        }
        reader__.erase(ctx);
        mu__.unlock();
        return;
    }
    this_coroutine::yield_coroutine();
}

inline void co_recursive_mutex::lock()
{
    auto ctx = get_co_env()->get_curr_co();
    while (true)
    {
        mu__.lock();
        if (owner__ != nullptr && owner__ != ctx)
        {
            mu__.unlock();
            this_coroutine::yield_coroutine();
            continue;
        }
        owner__ = ctx;
        ++count__;
        mu__.unlock();
        return;
    }
}

inline void co_recursive_mutex::unlock()
{
    auto ctx = get_co_env()->get_curr_co();
    while (true)
    {
        mu__.lock();
        if (owner__ != ctx)
        {
            mu__.unlock();
            this_coroutine::yield_coroutine();
            continue;
        }
        --count__;
        if (count__ == 0)
        {
            owner__ = nullptr;
        }
        mu__.unlock();
        return;
    }
    this_coroutine::yield_coroutine();
}

inline bool co_recursive_mutex::try_lock()
{
    auto ctx = get_co_env()->get_curr_co();
    if (!mu__.try_lock())
    {
        return false;
    }
    if (owner__ != nullptr && owner__ != ctx)
    {
        mu__.unlock();
        return false;
    }
    owner__ = ctx;
    ++count__;
    mu__.unlock();
    return true;
}

inline void co_timed_mutex::lock()
{
    mu__.lock();
}

inline bool co_timed_mutex::try_lock()
{
    return mu__.try_lock();
}

template <typename T>
inline bool co_timed_mutex::try_lock_for(const T& t)
{
    return try_lock_until(std::chrono::system_clock::now() + t);
}

template <typename T>
inline bool co_timed_mutex::try_lock_until(const T& t)
{
    while (std::chrono::system_clock::now() < t)
    {
        if (mu__.try_lock())
        {
            return true;
        }
    }
    return false;
}

inline void co_timed_mutex::unlock()
{
    mu__.unlock();
}

inline void co_recursive_timed_mutex::lock()
{
    mu__.lock();
}

inline bool co_recursive_timed_mutex::try_lock()
{
    return mu__.try_lock();
}

template <typename T>
bool co_recursive_timed_mutex::try_lock_for(const T& t)
{
    return try_lock_until(std::chrono::system_clock::now() + t);
}

template <typename T>
inline bool co_recursive_timed_mutex::try_lock_until(const T& t)
{
    while (std::chrono::system_clock::now() < t)
    {
        if (mu__.try_lock())
        {
            return true;
        }
    }
    return false;
}

inline void co_recursive_timed_mutex::unlock()
{
    mu__.unlock();
}

inline void co_shared_timed_mutex::lock()
{
    mu__.lock();
}

inline bool co_shared_timed_mutex::try_lock()
{
    return mu__.try_lock();
}

template <typename T>
inline bool co_shared_timed_mutex::try_lock_for(const T& t)
{
    return try_lock_until(std::chrono::system_clock::now() + t);
}

template <typename T>
inline bool co_shared_timed_mutex::try_lock_until(const T& t)
{
    while (std::chrono::system_clock::now() < t)
    {
        if (mu__.try_lock())
        {
            return true;
        }
    }
    return false;
}

inline void co_shared_timed_mutex::unlock()
{
    mu__.unlock();
}

inline void co_shared_timed_mutex::lock_shared()
{
    mu__.lock_shared();
}

inline bool co_shared_timed_mutex::try_lock_shared()
{
    return mu__.try_lock_shared();
}

template <typename T>
bool co_shared_timed_mutex::try_lock_shared_for(const T& t)
{
    return try_lock_shared_until(std::chrono::system_clock::now() + t);
}

template <typename T>
bool co_shared_timed_mutex::try_lock_shared_until(const T& t)
{
    while (std::chrono::system_clock::now() < t)
    {
        if (mu__.try_lock_shared())
        {
            return true;
        }
    }
    return false;
}

inline void co_shared_timed_mutex::unlock_shared()
{
    mu__.unlock_shared();
}

template <typename Function, typename... Args>
requires ReturnNotVoid<Function, Args...>
    std::future<std::invoke_result_t<std::decay_t<Function>, std::decay_t<Args>...>>
    co_async(Function&& f, Args&&... args)
{
}

template <typename Function, typename... Args>
requires ReturnVoid<Function, Args...>
    std::future<void>
    co_async(Function&& f, Args&&... args)
{
}

}