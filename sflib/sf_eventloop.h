
/**
* @version 1.0.0
* @author skyfire
* @file sf_eventloop.h
*/

/*
 * eventloop 事件循环
 */

#pragma once

#include "sf_stdc++.h"

#include "sf_nocopy.h"
#include "sf_object_msg_queue.hpp"

namespace skyfire
{
/**
 *  @brief 消息循环
 */
class eventloop final : nocopy<>
{
private:
    object_msg_queue* __p_msg_queue__ = object_msg_queue::instance();
    std::atomic<int>  running__ { 0 };

public:
    /**
     * @brief eventloop 构造一个事件循环对象
     */
    eventloop() = default;

    /**
     * @brief exec 执行事件循环
     */
    void exec();

    /**
     * @brief clear 清空事件
     */
    void clear() const;

    /**
     * @brief wake 激活事件（通常用于退出）
     */
    void wake() const;

    /**
     * @brief quit 退出事件循环
     */
    void quit();
};

} // namespace skyfire
