#pragma once

namespace skyfire
{

enum class co_future_status
{
    deferred,
    ready,
    timeout
};

template <typename T>
class co_promise;

// todo 实现
template <typename T>
class co_future
{
private:
    std::unique_ptr<T> data__;

    void set_data__(const T& t);

public:
    T    get();
    bool valid();
    void wait() const;
    template <typename Tm>
    co_future_status wait_for(const Tm& tm);
    template <typename Tm>
    co_future_status wait_until(const Tm& tm);

    friend co_promise<T>;
};
}