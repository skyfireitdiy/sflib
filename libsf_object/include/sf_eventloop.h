/*
 * sf_eventloop 事件循环
 */

#pragma once

#include <condition_variable>
#include <mutex>
#include <atomic>

#include "sf_single_instance.h"
#include "sf_msg_queue.h"
#include "sf_nocopy.h"

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
        sf_eventloop(){

        }

        /**
         * @brief exec 执行事件循环
         */
        void exec(){
            running__ = true;
            while(true)
            {
                if(running__ == false)
                {
                    break;
                }
                if(__p_msg_queue__->empty())
                {
                    __p_msg_queue__->wait_msg();
                }
                if(!__p_msg_queue__->empty())
                {
                    auto func = __p_msg_queue__->take();
                    if(!func)
                    {
                        continue;
                    }
                    else
                    {
                        func();
                    }
                }
            }
        }

        /**
         * @brief clear 清空事件
         */
        void clear()
        {
            __p_msg_queue__->clear();
        }

        /**
         * @brief wake 激活事件（通常用于退出）
         */
        void wake()
        {
            __p_msg_queue__->add_empty_msg();
        }

        /**
         * @brief quit 退出事件循环
         */
        void quit()
        {
            running__ = false;
            wake();
        }
    };


}
