
/**
* @version 1.0.0
* @author skyfire
* @file sf_msg_queue.h
*/

/*
 * sf_msg_queue 消息队列
 */

#pragma once

#include "sf_stdc++.h"
#include "tools/sf_single_instance.hpp"

namespace skyfire {
/**
 *  @brief 消息队列
 */
template <typename T>
class sf_msg_queue {
private:
    std::list<T> data__;
    std::mutex mu_data_op__;

    std::condition_variable wait_cond__;

public:
    /**
     * 增加消息
     * @param msg 要调用的函数
     */
    void add_msg(T msg);

    /**
     * 删除消息
     * @param op 判断是否删除的谓词
     */
    void remove_msg(std::function<bool(const T&)> op);

    /**
     * 清空消息
     */
    void clear();

    /**
     * 获取一条消息
     * @return 返回消息，如果没有消息，返回nullptr
     */
    std::shared_ptr<T> take_msg();

    /**
     * @brief 获取所有消息
     * 
     * @return std::list<T> 所有消息
     */
    std::list<T> take_all_msg();

    /**
     * 判断是否队列为空
     * @return 是否为空
     */
    bool empty() const;

    /**
     * 等待新消息到来
     */
    void wait_new_msg();

    /**
     * 等待消息(有消息会直接返回)
     */
    void wait_msg();

    /**
     * 添加一个空消息
     */
    void add_empty_msg();
};

} // namespace skyfire
