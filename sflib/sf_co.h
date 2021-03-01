#pragma once

#include "sf_stdc++.h"
#include <time.h>

namespace skyfire
{

#define SIG_CO_SCHE (SIGRTMIN + 1)

constexpr int default_co_stack_size = 1024 * 2;

class co_ctx;
class co_env;
class co_manager;

enum class co_state
{
    ready,
    running,
    suspended,
    finished
};

class coroutine
{
private:
    co_ctx* ctx__;
    bool    detached__ = false;
    bool    joined__   = false;
    bool    invalid__  = false;

    coroutine(const coroutine& ctx)             = delete;
    coroutine& operator==(const coroutine& ctx) = delete;

public:
    template <typename Tm>
    bool wait_for(Tm t);
    template <typename Tm>
    bool wait_until(Tm expire);
    void wait();
    void join();
    void detach();
    bool valid() const;

    template <typename Func, typename... Args>
    coroutine(Func func, Args&&... args);
    template <typename Func, typename... Args>
    coroutine(size_t default_stack_size, Func func, Args&&... args);
    coroutine(co_ctx* ctx);
};

co_ctx* get_current_coroutine();
void    release_curr_co();
void    yield_coroutine();

}
