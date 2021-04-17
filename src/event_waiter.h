
#pragma once
#include "co_condition_variable.h"
#include "co_mutex.h"
#include "meta.hpp"
#include "nocopy.h"
#include "object.hpp"
#include "stdc++.h"

#define sf_wait(obj, name)                                                         \
    {                                                                              \
        auto p_waiter = skyfire::make_waiter((obj)->__##name##_signal_func_vec__); \
        auto bind_id  = sf_bind(obj, name, p_waiter->make_quit_func__(), true);    \
        p_waiter->wait__();                                                        \
        sf_unbind(obj, name, bind_id);                                             \
    }
namespace skyfire
{
template <typename... ARGS>
class __event_waiter__ : public nocopy<>
{
private:
    co_mutex              mu_cond__;
    co_condition_variable cond__;

public:
    auto make_quit_func__();
    void wait__();
    void quit__();
};
template <typename... ARGS>
std::shared_ptr<__event_waiter__<ARGS...>> make_waiter(
    const std::vector<std::tuple<std::function<void(ARGS...)>, bool, int>>&);
} // namespace skyfire
