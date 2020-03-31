
/**
* @version 1.0.0
* @author skyfire
* @file sf_msg_queue.hpp
*/

/*
 * sf_msg_queue 消息队列
 */

#pragma once

#include "sf_msg_queue.h"

namespace skyfire {

template <typename T>
void sf_msg_queue<T>::add_msg(T msg)
{
    std::lock_guard<std::mutex> lck(mu_data_op__);
    data__.emplace_back(msg);
    wait_cond__.notify_one();
}

template <typename T>
void sf_msg_queue<T>::remove_msg(std::function<bool(const T&)> op)
{
    std::lock_guard<std::mutex> lck(mu_data_op__);
    data__.remove_if(op);
}

template <typename T>
void sf_msg_queue<T>::clear()
{
    std::lock_guard<std::mutex> lck(mu_data_op__);
    data__.clear();
}

template <typename T>
std::shared_ptr<T> sf_msg_queue<T>::take_msg()
{
    std::lock_guard<std::mutex> lck(mu_data_op__);
    if (data__.empty()) {
        return nullptr;
    }
    auto ret = std::make_shared<T>(data__.front());
    data__.pop_front();
    return ret;
}

template <typename T>
bool sf_msg_queue<T>::empty() const { return data__.empty(); }

template <typename T>
void sf_msg_queue<T>::wait_new_msg()
{
    std::unique_lock<std::mutex> lck(mu_data_op__);
    wait_cond__.wait(lck);
}

template <typename T>
std::list<T> sf_msg_queue<T>::take_all_msg()
{
    std::lock_guard<std::mutex> lck(mu_data_op__);
    return std::move(data__);
}

template <typename T>
void sf_msg_queue<T>::wait_msg()
{
    std::unique_lock<std::mutex> lck(mu_data_op__);
    wait_cond__.wait(lck, [this]() -> bool { return !data__.empty(); });
}

template <typename T>
void sf_msg_queue<T>::add_empty_msg() { wait_cond__.notify_all(); }
} // namespace skyfire
