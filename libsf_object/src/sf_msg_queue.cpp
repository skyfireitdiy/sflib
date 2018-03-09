#include "sf_msg_queue.h"
#include <algorithm>

namespace skyfire
{
    void sf_msg_queue::add_msg(void *id, std::function<void()> func)
    {
        std::lock_guard<std::mutex> lck(mu_func_data_op__);
        func_data__.push_back({id, func});
    }

    void sf_msg_queue::remove_msg(void *id)
    {
        std::lock_guard<std::mutex> lck(mu_func_data_op__);
        std::remove_if(func_data__.begin(),func_data__.end(),[=](const std::pair<void*,std::function<void()>> dt){
            return dt.first == id;
        });
    }

    void sf_msg_queue::clear()
    {
        std::lock_guard<std::mutex> lck(mu_func_data_op__);
        func_data__.clear();
    }

    std::function<void()> sf_msg_queue::take()
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

    bool sf_msg_queue::empty()
    {
        return func_data__.empty();
    }


    sf_msg_queue::sf_msg_queue()
    {

    }

}