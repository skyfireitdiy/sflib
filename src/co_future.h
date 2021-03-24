#pragma once

#include "co_utils.h"
#include "stdc++.h"
namespace skyfire
{

template <typename T>
class co_promise;

template <typename T>
class co_future
{
private:
    std::atomic<bool>  valid__     = { false };
    std::atomic<bool>  has_value__ = { false };
    std::atomic<bool>  has_exp__   = { false };

    std::exception_ptr exp__;
    std::any value__;

public:
    T    get();
    bool valid() const;
    void wait() const;
    template <typename Tm>
    bool wait_for(const Tm& tm);
    template <typename Tm>
    bool wait_until(const Tm& tm);
    friend co_promise<T>;
};

}