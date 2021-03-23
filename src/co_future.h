#pragma once

#include "co_utils.h"
#include "stdc++.h"
namespace skyfire
{

template <typename T>
class co_promise;

class co_future_base
{
protected:
    std::atomic<bool> valid__     = { false };
    std::atomic<bool> has_value__ = { false };

public:
    bool valid() const;
    void wait() const;
    template <typename Tm>
    bool wait_for(const Tm& tm);
    template <typename Tm>
    bool wait_until(const Tm& tm);
};

template <typename T>
class co_future : public co_future_base
{
private:
    std::unique_ptr<T> data__;

    void set_data__(const T& t);
    bool has_value__() const;

public:
    T get();

    friend co_promise<T>;
};

template <>
class co_future<void> : public co_future_base
{
private:
    void set_data__();
    bool has_value__();

    friend co_promise<void>;
};

}