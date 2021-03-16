#pragma once

#include "sf_stdc++.h"

namespace skyfire
{

#define SIG_CO_SCHE (SIGRTMIN + 1)

constexpr int default_co_stack_size = 1024 * 1024 * 2;

class co_ctx;
class co_env;
class co_manager;

template <typename Function, typename... Args>
concept ReturnVoid = std::is_same_v<std::invoke_result_t<std::decay_t<Function>, std::decay_t<Args>...>, void>;

template <typename Function, typename... Args>
concept ReturnNotVoid = !std::is_same_v<std::invoke_result_t<std::decay_t<Function>, std::decay_t<Args>...>, void>;

enum class co_state
{
    ready,
    running,
    suspended,
    finished
};

struct coroutine_attr
{
    size_t      stack_size   = default_co_stack_size;
    bool        shared_stack = false;
    std::string name         = "unamed";
};

template <typename Func, typename... Args>
class coroutine final
{
private:
    co_ctx*                                                                       ctx__;
    bool                                                                          detached__ = false;
    bool                                                                          joined__   = false;
    bool                                                                          invalid__  = false;
    std::promise<std::invoke_result_t<std::decay_t<Func>, std::decay_t<Args>...>> promise__;

    coroutine(const coroutine& ctx)             = delete;
    coroutine& operator==(const coroutine& ctx) = delete;

public:
    coroutine(coroutine&& other);
    coroutine& operator=(coroutine&& other);

    template <typename T>
    bool wait_for(const T& t);
    template <typename T>
    bool wait_until(const T& expire);
    void wait();
    void join();
    void detach();
    bool valid() const;
    bool joinable() const;

    coroutine(Func&& func, Args&&... args) requires ReturnVoid<Func, Args...>;
    coroutine(const coroutine_attr& attr, Func&& func, Args&&... args) requires ReturnVoid<Func, Args...>;

    coroutine(Func&& func, Args&&... args) requires ReturnNotVoid<Func, Args...>;
    coroutine(const coroutine_attr& attr, Func&& func, Args&&... args) requires ReturnNotVoid<Func, Args...>;

    ~coroutine();
};

namespace co
{
    long long   get_id();
    std::string get_name();

    template <typename T>
    void sleep_for(const T& t);

    template <typename T>
    void sleep_until(const T& t);

    void yield_coroutine();
};

template <typename Func, typename... Args>
requires ReturnVoid<Func, Args...>
coroutine(Func&&, Args&&...) -> coroutine<Func, Args...>;

template <typename Func, typename... Args>
requires ReturnVoid<Func, Args...>
coroutine(const coroutine_attr&, Func&&, Args&&...) -> coroutine<Func, Args...>;

template <typename Func, typename... Args>
requires ReturnNotVoid<Func, Args...>
coroutine(Func&&, Args&&...) -> coroutine<Func, Args...>;

template <typename Func, typename... Args>
requires ReturnNotVoid<Func, Args...>
coroutine(const coroutine_attr&, Func&&, Args&&...) -> coroutine<Func, Args...>;

// template <typename Function, typename... Args>
// requires ReturnNotVoid<Function, Args...>
// std::future<std::invoke_result_t<std::decay_t<Function>, std::decay_t<Args>...>>
// co_async(Function&& f, Args&&... args);

// template <typename Function, typename... Args>
// requires ReturnVoid<Function, Args...>
// std::future<void>
// co_async(Function&& f, Args&&... args);

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

}
