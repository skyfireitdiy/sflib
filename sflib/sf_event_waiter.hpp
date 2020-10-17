
/**
* @version 1.0.0
* @author skyfire
* @file __event_waiter__.hpp
*/

/*
 * __event_waiter__ 事件等待
 */
#pragma once
#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCUnusedGlobalDeclarationInspection"

#include "sf_event_waiter.h"

namespace skyfire {

template <typename... ARGS>
void __event_waiter__<ARGS...>::wait__()
{
    std::unique_lock<std::mutex> lck(mu_cond__);
    cond__.wait(lck);
}

template <typename... ARGS>
auto __event_waiter__<ARGS...>::make_quit_func__()
{
    return [this](ARGS...) { quit__(); };
}

template <typename... ARGS>
void __event_waiter__<ARGS...>::quit__()
{
    cond__.notify_one();
}

template <typename... ARGS>
std::shared_ptr<__event_waiter__<ARGS...>> make_waiter(
    const std::vector<std::tuple<std::function<void(ARGS...)>, bool, int>>&)
{
    return std::make_shared<__event_waiter__<ARGS...>>();
}
} // namespace skyfire

#pragma clang diagnostic pop