
/**
* @version 1.0.0
* @author skyfire
* @file sf_event_waiter.hpp
*/

/*
 * sf_event_waiter 事件等待
 */
#pragma once
#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCUnusedGlobalDeclarationInspection"

#include "sf_event_waiter.h"

namespace skyfire {

template <typename... ARGS>
void sf_event_waiter<ARGS...>::wait()
{
    std::unique_lock<std::mutex> lck(mu_cond__);
    cond__.wait(lck);
}

template <typename... ARGS>
auto sf_event_waiter<ARGS...>::make_quit_func__()
{
    return [this](ARGS...) { quit(); };
}

template <typename... ARGS>
void sf_event_waiter<ARGS...>::quit()
{
    cond__.notify_one();
}

template <typename... ARGS>
std::shared_ptr<sf_event_waiter<ARGS...>> sf_make_waiter(
    const std::vector<std::tuple<std::function<void(ARGS...)>, bool, int>>&)
{
    return std::make_shared<sf_event_waiter<ARGS...>>();
}
} // namespace skyfire

#pragma clang diagnostic pop