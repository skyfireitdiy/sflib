
/**
* @version 1.0.0
* @author skyfire
* @mail skyfireitdiy@hotmail.com
* @see http://github.com/skyfireitdiy/sflib
* @file sf_msg_queue.hpp

* sflib第一版本发布
* 版本号1.0.0
* 发布日期：2018-10-22
*/

/*
 * sf_msg_queue 消息队列
 */

#pragma once

#include "sf_msg_queue.h"

namespace skyfire {

inline void sf_msg_queue::add_msg(void *id, std::function<void()> func) {
    std::lock_guard<std::mutex> lck(mu_func_data_op__);
    func_data__.emplace_back(id, func);
    wait_cond__.notify_all();
}

inline void sf_msg_queue::remove_msg(void *id) {
    std::lock_guard<std::mutex> lck(mu_func_data_op__);
    func_data__.remove_if(
        [=](const std::pair<void *, std::function<void()>> dt) {
            return dt.first == id;
        });
}

inline void sf_msg_queue::clear() {
    std::lock_guard<std::mutex> lck(mu_func_data_op__);
    func_data__.clear();
}

inline std::function<void()> sf_msg_queue::take() {
    std::lock_guard<std::mutex> lck(mu_func_data_op__);
    if (func_data__.empty()) {
        return std::function<void()>();
    }
    auto ret = func_data__.begin()->second;
    func_data__.pop_front();
    return ret;
}

inline bool sf_msg_queue::empty() const { return func_data__.empty(); }

inline void sf_msg_queue::wait_msg() {
    std::unique_lock<std::mutex> lck(mu_func_data_op__);
    wait_cond__.wait(lck);
}

inline void sf_msg_queue::add_empty_msg() { wait_cond__.notify_all(); }
}    // namespace skyfire
