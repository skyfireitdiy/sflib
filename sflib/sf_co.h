#pragma once

#include "sf_stdc++.h"

namespace skyfire
{

#define SIG_CO_SCHE (SIGRTMIN + 1)

class co_manager
{
    std::unordered_set<pthread_t> co_thread_id__;
    mutable std::mutex            mu_co_thread_id_set__;

public:
    std::unordered_set<pthread_t> get_co_thread_ids() const;
    void                          add_thread_id(pthread_t th);
    void                          remove_thread_id(pthread_t th);
};

class co_ctx
{
    void*                 regs[14] = {};
    bool                  main;
    bool                  running = false;
    std::vector<char>     stack_data;
    size_t                stack_size;
    std::function<void()> entry;

public:
    co_ctx(bool main = false, size_t ss = 2048)
        : main(main)
        , stack_data(ss)
        , stack_size(ss)
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
    void                    create_coroutine(std::function<void()> func);
    void                    yield_coroutine();
};

template <typename Func, typename... Args>
void                    create_coroutine(Func func, Args&&... args);
std::shared_ptr<co_ctx> get_current_coroutine();
void                    release_curr_co();
void                    yield_coroutine();

}