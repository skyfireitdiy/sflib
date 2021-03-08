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

    template <typename T>
    bool wait_for(const T& t);
    template <typename T>
    bool      wait_until(const T& expire);
    void      wait();
    void      join();
    void      detach();
    bool      valid() const;
    static long long get_id();
    bool      joinable() const;

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
    co_ctx*                     writer__ { nullptr };
    co_mutex                    mu__;

    co_shared_mutex(const co_shared_mutex&) = delete;
    co_shared_mutex& operator=(const co_shared_mutex&) = delete;
    co_shared_mutex(co_shared_mutex&&)                 = delete;
    co_shared_mutex& operator=(co_shared_mutex&&) = delete;

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
private:
    co_recursive_mutex(const co_recursive_mutex&) = delete;
    co_recursive_mutex& operator=(const co_recursive_mutex&) = delete;
    co_recursive_mutex(co_recursive_mutex&&)                 = delete;
    co_recursive_mutex& operator=(co_recursive_mutex&&) = delete;

    co_mutex mu__;
    co_ctx*  owner__ { nullptr };
    size_t   count__ { 0 };

public:
    void lock();
    void unlock();
    bool try_lock();
};

class co_timed_mutex final
{
private:
    co_timed_mutex(const co_timed_mutex&) = delete;
    co_timed_mutex& operator=(const co_timed_mutex&) = delete;
    co_timed_mutex(co_timed_mutex&&)                 = delete;
    co_timed_mutex& operator=(co_timed_mutex&&) = delete;

    co_mutex mu__;

public:
    void lock();
    bool try_lock();
    template <typename T>
    bool try_lock_for(const T& t);
    template <typename T>
    bool try_lock_until(const T& t);
    void unlock();
};

class co_recursive_timed_mutex final
{
private:
    co_recursive_timed_mutex(const co_recursive_timed_mutex&) = delete;
    co_recursive_timed_mutex& operator=(const co_recursive_timed_mutex&) = delete;
    co_recursive_timed_mutex(co_recursive_timed_mutex&&)                 = delete;
    co_recursive_timed_mutex& operator=(co_recursive_timed_mutex&&) = delete;

    co_recursive_mutex mu__;

public:
    void lock();
    bool try_lock();
    template <typename T>
    bool try_lock_for(const T& t);
    template <typename T>
    bool try_lock_until(const T& t);
    void unlock();
};

class co_shared_timed_mutex final
{
private:
    co_shared_timed_mutex(const co_shared_timed_mutex&) = delete;
    co_shared_timed_mutex& operator=(const co_shared_timed_mutex&) = delete;
    co_shared_timed_mutex(co_shared_timed_mutex&&)                 = delete;
    co_shared_timed_mutex& operator=(co_shared_timed_mutex&&) = delete;

    co_shared_mutex mu__;

public:
    void lock();
    bool try_lock();
    template <typename T>
    bool try_lock_for(const T& t);
    template <typename T>
    bool try_lock_until(const T& t);
    void unlock();
    void lock_shared();
    bool try_lock_shared();
    template <typename T>
    bool try_lock_shared_for(const T& t);
    template <typename T>
    bool try_lock_shared_until(const T& t);
    void unlock_shared();
};

void yield_coroutine();

}
