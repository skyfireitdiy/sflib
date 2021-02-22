#pragma once

#include "sf_stdc++.h"
#include <time.h>

namespace skyfire
{

#define SIG_CO_SCHE (SIGRTMIN + 1)

constexpr int default_co_stack_size = 1024 * 2;

class co_manager
{
    std::unordered_set<pthread_t> co_thread_id__;
    mutable std::mutex            mu_co_thread_id_set__;
    timer_t                       timer__;
    struct itimerspec             ts__
    {
        { 0, 0 }, { 0, 1000'000'00 }
    };

public:
    std::unordered_set<pthread_t> get_co_thread_ids() const;
    void                          add_thread_id(pthread_t th);
    void                          remove_thread_id(pthread_t th);
    co_manager();
    void reset_timer();
};

enum class co_state
{
    ready,
    running,
    suspended,
    finished
};

class co_ctx
{
    void*                 regs__[14] = {};
    bool                  main__;
    co_state              state__ = co_state::ready;
    std::vector<char>     stack_data__;
    size_t                stack_size__;
    std::function<void()> entry__;

public:
    co_ctx(bool main = false, size_t ss = default_co_stack_size)
        : main__(main)
        , stack_data__(ss)
        , stack_size__(ss)
    {
    }

    std::function<void()> get_entry() const;

    friend class co_env;
};

struct co_env : public std::enable_shared_from_this<co_env>
{
private:
    std::vector<std::shared_ptr<co_ctx>> co_set__;
    int                                  curr_co_index__ = 0;
    std::shared_ptr<co_ctx>              current_co__    = nullptr;

    std::shared_ptr<co_ctx> choose_co__();
    void                    append_co__(std::shared_ptr<co_ctx> ctx);

public:
    co_env();
    ~co_env();
    std::shared_ptr<co_ctx> choose_co();
    void                    release_curr_co();
    std::shared_ptr<co_ctx> get_current_coroutine() const;
    std::shared_ptr<co_ctx> create_coroutine(size_t default_stack_size, std::function<void()> func);
    void                    yield_coroutine();
    void                    wait_coroutine(std::shared_ptr<co_ctx> ctx);
};

template <typename Func, typename... Args>
std::shared_ptr<co_ctx> create_coroutine(Func func, Args&&... args);
template <typename Func, typename... Args>
std::shared_ptr<co_ctx> create_coroutine(size_t default_stack_size, Func func, Args&&... args);
std::shared_ptr<co_ctx> get_current_coroutine();
void                    release_curr_co();
void                    yield_coroutine();
void                    wait_coroutine(std::shared_ptr<co_ctx> ctx);

}