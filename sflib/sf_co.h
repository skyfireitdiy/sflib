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

template <typename Func, typename... Args>
co_ctx* create_coroutine(Func func, Args&&... args);
template <typename Func, typename... Args>
co_ctx* create_coroutine(size_t default_stack_size, Func func, Args&&... args);
co_ctx* get_current_coroutine();
void    release_curr_co();
void    yield_coroutine();
void    wait_coroutine(co_ctx* ctx);
template <typename Tm>
bool wait_coroutine_for(co_ctx* ctx, Tm t);
template <typename Tm>
bool wait_coroutine_until(co_ctx* ctx, Tm expire);

}