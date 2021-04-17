#pragma once

#include "co_ctx.h"
#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <sys/epoll.h>
#include <sys/resource.h>
#include <sys/socket.h>
#include <sys/sysinfo.h>
#include <sys/types.h>
#include <unistd.h>

namespace skyfire
{

int co_epoll_wait(int __epfd, epoll_event* __events,
                  int __maxevents, int __timeout);

void __switch_co__(co_ctx* curr, co_ctx* next);
void __co_func__(co_ctx*);
void __co_save_stack__();
void __swap_regs__(const void*, const void*);

namespace this_coroutine
{
    long long   get_id();
    std::string get_name();

    template <typename T>
    void sleep_for(const T& t);

    template <typename T>
    void sleep_until(const T& t);

    void yield_coroutine();

    template <typename T, typename... Args>
    T& co_local(const std::string& name, Args&&... args);
};

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

}