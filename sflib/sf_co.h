#pragma once

#include "sf_stdc++.h"
#include <time.h>

namespace skyfire
{

#define SIG_CO_SCHE (SIGRTMIN + 1)

constexpr int default_co_stack_size = 1024 * 1024 * 2;

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

class coroutine final
{
private:
    co_ctx* ctx__;
    bool    detached__ = false;
    bool    joined__   = false;
    bool    invalid__  = false;

    coroutine(const coroutine& ctx)             = delete;
    coroutine& operator==(const coroutine& ctx) = delete;

public:
    coroutine(coroutine&& other);
    coroutine& operator=(coroutine&& other);

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
    ~coroutine();
};

class co_mutex final
{
private:
    co_mutex(const co_mutex&) = delete;
    co_mutex& operator=(const co_mutex&) = delete;
    co_mutex(co_mutex&&)                 = delete;
    co_mutex& operator=(co_mutex&&) = delete;

    std::atomic<co_ctx*> owner__ { nullptr };

public:
    co_mutex() = default;
    void lock();
    void unlock();
    bool try_lock();
};

class co_shared_mutex final
{
private:
    std::unordered_set<co_ctx*> reader__;
    co_ctx*                     writer__;
    co_mutex                    mu__;

    co_shared_mutex(const co_mutex&) = delete;
    co_shared_mutex& operator=(const co_mutex&) = delete;
    co_shared_mutex(co_mutex&&)                 = delete;
    co_shared_mutex& operator=(co_mutex&&) = delete;

public:
    void lock();
    bool try_lock();
    void unlock();

    void lock_shared();
    bool try_lock_shared();
    void unlock_shared();
};

class co_recursive_mutex final
{
public:
    void lock();
    void unlock();
    bool try_lock();
};

class co_timed_mutex final
{
public:
    void lock();
    bool try_lock();
    bool try_lock_for();
    bool try_lock_until();
    void unlock();
};

class co_recursive_timed_mutex final
{
public:
    void lock();
    bool try_lock();
    bool try_lock_for();
    bool try_lock_until();
    void unlock();
};

class co_shared_timed_mutex final
{
public:
    void lock();
    bool try_lock();
    bool try_lock_for();
    bool try_lock_until();
    void unlock();
    void lock_shared();
    bool try_lock_shared();
    bool try_lock_shared_for();
    bool try_lock_shared_until();
    void unlock_shared();
};

void      yield_coroutine();
long long get_coroutine_id();
}
