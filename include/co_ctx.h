#pragma once
#include "co_attr.h"
#include "co_env.h"
#include <bits/stdc++.h>

namespace skyfire
{

constexpr int co_retindex = 5;
constexpr int co_rspindex = 7;
constexpr int co_rdiindex = 8;
constexpr int co_rbpindex = 4;

template <typename T>
concept VoidType = std::is_same_v<T, void>;

template <typename T>
concept NotVoidType = !std::is_same_v<T, void>;

template <typename Function, typename... Args>
concept ReturnVoid = std::is_same_v<std::invoke_result_t<std::decay_t<Function>, std::decay_t<Args>...>, void>;

template <typename Function, typename... Args>
concept ReturnNotVoid = !std::is_same_v<std::invoke_result_t<std::decay_t<Function>, std::decay_t<Args>...>, void>;

template <typename Func>
concept ReturnBool = std::is_same_v<std::invoke_result_t<std::decay_t<Func>>, bool>;

enum class co_state
{
    ready,
    running,
    suspended,
    finished
};

class co_ctx final
{
    const void*                               regs__[9]    = {};
    std::atomic<co_state>                     state__      = co_state::ready;
    char*                                     stack_data__ = nullptr;
    size_t                                    stack_size__;
    std::function<void()>                     entry__;
    bool                                      shared_stack__;
    std::vector<char>                         saved_stack__;
    std::string                               name__;
    std::atomic<bool>                         wait_cond__ { false };
    std::recursive_mutex                      mu_co_data__;
    std::unordered_map<std::string, std::any> co_data__;

public:
    co_ctx(std::function<void()> entry, const co_attr_config& attr);

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
    bool        shared_stack() const;
    void        save_stack();
    void        restore_stack();
    void        set_curr_sp(char* sp);
    std::string get_name() const;
    long long   get_id() const;
    void        set_wait_cond();
    void        unset_wait_cond();
    void        wait_cond();
    void        wait_cond(std::function<bool()> cond);
    template <typename T>
    bool wait_cond_until(const T& tm, std::function<bool()> cond);
    template <typename T, typename... Args>
    std::shared_ptr<T> get_co_data(const std::string& name, Args&&... args);
};

void __switch_co__(co_ctx* curr, co_ctx* next);
void __co_func__(co_ctx*);
void __co_save_stack__();
void __swap_regs__(const void*, const void*);

// impl

template <typename T>
bool co_ctx::wait_cond_until(const T& tm, std::function<bool()> cond)
{
    if (cond())
    {
        wait_cond__ = false;
        return true;
    }
    while (wait_cond__)
    {
        if (cond())
        {
            return true;
        }
        if (std::chrono::system_clock::now() < tm)
        {
            get_co_env()->yield_coroutine();
        }
        wait_cond__ = false;
        return false;
    }
    return true;
}

template <typename T, typename... Args>
std::shared_ptr<T> co_ctx::get_co_data(const std::string& name, Args&&... args)
{
    std::lock_guard<std::recursive_mutex> lck(mu_co_data__);
    if (!co_data__.contains(name))
    {
        co_data__[name] = std::shared_ptr<T>(new T(std::forward<Args>(args)...));
    }
    return std::any_cast<std::shared_ptr<T>>(co_data__[name]);
}
}
