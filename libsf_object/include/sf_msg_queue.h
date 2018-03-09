#pragma once

#include "sf_single_instance.h"
#include <functional>
#include <list>
#include <map>
#include <mutex>

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
    public:
        void add_msg(void *id, std::function<void()> func);
        void remove_msg(void *id);
        void clear();
        std::function<void()> take();
        bool empty();
    };
}