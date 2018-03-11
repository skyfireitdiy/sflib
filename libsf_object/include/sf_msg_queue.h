#pragma once

#include "sf_single_instance.h"
#include <functional>
#include <list>
#include <map>
#include <mutex>
#include <algorithm>

namespace skyfire
{
    class sf_msg_queue
    {
    public:
        SF_SINGLE_TON(sf_msg_queue);
    private:
        sf_msg_queue(){}
        std::list<std::pair<void*,std::function<void()>>> func_data__;
        std::mutex mu_func_data_op__;
    public:
        void add_msg(void *id, std::function<void()> func)
        {
            std::lock_guard<std::mutex> lck(mu_func_data_op__);
            func_data__.push_back({id, func});
        }


        void remove_msg(void *id)
        {
            std::lock_guard<std::mutex> lck(mu_func_data_op__);
            std::remove_if(func_data__.begin(),func_data__.end(),[=](const std::pair<void*,std::function<void()>> dt){
                return dt.first == id;
            });
        }

        void clear()
        {
            std::lock_guard<std::mutex> lck(mu_func_data_op__);
            func_data__.clear();
        }

        std::function<void()> take()
        {
            std::lock_guard<std::mutex> lck(mu_func_data_op__);
            if(func_data__.empty())
            {
                return std::function<void()>();
            }
            auto ret = func_data__.begin()->second;
            func_data__.pop_front();
            return ret;
        }


        bool empty()
        {
            return func_data__.empty();
        }
    };
}