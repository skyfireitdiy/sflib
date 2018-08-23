/*
 * sf_eventloop 事件循环
 */

#pragma once

#include <condition_variable>
#include <mutex>
#include <atomic>

#include "sf_single_instance.hpp"
#include "sf_msg_queue.hpp"
#include "sf_nocopy.hpp"

namespace skyfire
{
    class sf_eventloop : sf_nocopy<>
    {
    private:
        sf_msg_queue* __p_msg_queue__ = sf_msg_queue::get_instance();
        std::atomic<int> running__ { 0 };
    public:
        /**
         * @brief sf_eventloop 构造一个事件循环对象
         */
        sf_eventloop();

        /**
         * @brief exec 执行事件循环
         */
        void exec();

        /**
         * @brief clear 清空事件
         */
        void clear();

        /**
         * @brief wake 激活事件（通常用于退出）
         */
        void wake();

        /**
         * @brief quit 退出事件循环
         */
        void quit();
    };


}
