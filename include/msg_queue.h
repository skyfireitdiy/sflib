
#pragma once
#include "co_condition_variable.h"
#include "co_mutex.h"
#include "single_instance.h"
#include <bits/stdc++.h>

namespace skyfire
{
template <typename T>
class msg_queue
{
private:
    std::list<T>          data__;
    co_mutex              mu_data_op__;
    co_condition_variable wait_cond__;

public:
    void               add_msg(T msg);
    void               remove_msg(std::function<bool(const T&)> op);
    void               clear();
    std::shared_ptr<T> take_msg();
    std::list<T>       take_all_msg();
    bool               empty() const;
    void               wait_new_msg();
    void               wait_msg();
    void               add_empty_msg();
};

// impl

template <typename T>
void msg_queue<T>::add_msg(T msg)
{
    std::lock_guard<co_mutex> lck(mu_data_op__);
    data__.emplace_back(msg);
    wait_cond__.notify_one();
}
template <typename T>
void msg_queue<T>::remove_msg(std::function<bool(const T&)> op)
{
    std::lock_guard<co_mutex> lck(mu_data_op__);
    data__.remove_if(op);
}
template <typename T>
void msg_queue<T>::clear()
{
    std::lock_guard<co_mutex> lck(mu_data_op__);
    data__.clear();
}
template <typename T>
std::shared_ptr<T> msg_queue<T>::take_msg()
{
    std::lock_guard<co_mutex> lck(mu_data_op__);
    if (data__.empty())
    {
        return nullptr;
    }
    auto ret = std::make_shared<T>(data__.front());
    data__.pop_front();
    return ret;
}
template <typename T>
bool msg_queue<T>::empty() const { return data__.empty(); }
template <typename T>
void msg_queue<T>::wait_new_msg()
{
    std::unique_lock<co_mutex> lck(mu_data_op__);
    wait_cond__.wait(lck);
}
template <typename T>
std::list<T> msg_queue<T>::take_all_msg()
{
    std::lock_guard<co_mutex> lck(mu_data_op__);
    return std::move(data__);
}
template <typename T>
void msg_queue<T>::wait_msg()
{
    std::unique_lock<co_mutex> lck(mu_data_op__);
    wait_cond__.wait(lck, [this]() -> bool { return !data__.empty(); });
}
template <typename T>
void msg_queue<T>::add_empty_msg()
{
    wait_cond__.notify_all();
}
} // namespace skyfire
