/*
 * sf_msg_queue 消息队列
 */

#pragma once

#include "sf_single_instance.hpp"
#include <functional>
#include <list>
#include <map>
#include <mutex>
#include <algorithm>
#include <condition_variable>

namespace skyfire
{
    class sf_msg_queue
    {
    public:
        SF_SINGLE_TON(sf_msg_queue);
    private:
        sf_msg_queue();
        std::list<std::pair<void*,std::function<void()>>> func_data__;
        std::mutex mu_func_data_op__;

        std::mutex wait_mu__;
        std::condition_variable wait_cond__;
    public:
        void add_msg(void *id, std::function<void()> func);


        void remove_msg(void *id);

        void clear();

        std::function<void()> take();


        bool empty();

        void wait_msg();

        void add_empty_msg();
    };

}
