#pragma once

#include "co_attr.h"
#include "co_ctx.h"
#include "co_mutex.h"
#include "co_utils.h"
#include "stdc++.h"

namespace skyfire
{

class coroutine final
{
private:
    co_ctx*        ctx__;
    bool           joined__  = false;
    bool           invalid__ = false;
    co_attr_config attr__;
    std::any       result__;
    // todo 需要替换为 co_promise

    coroutine(const coroutine& ctx)             = delete;
    coroutine& operator==(const coroutine& ctx) = delete;

public:
    coroutine(coroutine&& other);
    coroutine& operator=(coroutine&& other);

    template <typename T>
    bool wait_for(const T& t);
    template <typename T>
    bool        wait_until(const T& expire);
    void        wait();
    void        join();
    bool        valid() const;
    bool        joinable() const;
    void        manage_this_thread();
    long long   get_id() const;
    std::string get_name() const;

    template <typename T = void>
    T get();

    template <typename Func, typename... Args>
    coroutine(Func&& func, Args&&... args);
    template <typename Func, typename... Args>
    coroutine(const std::initializer_list<co_attr_option_type>& attr_config, Func&& func, Args&&... args);

    ~coroutine();
};

}
