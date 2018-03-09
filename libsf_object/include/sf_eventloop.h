#pragma once

#include <condition_variable>
#include <mutex>
#include <atomic>

#include "sf_single_instance.h"
#include "sf_msg_queue.h"

namespace skyfire
{
    class sf_eventloop
    {
    private:
        sf_eventloop(){}
        sf_msg_queue* __p_msg_queue__ = sf_msg_queue::get_instance();
        std::atomic<bool> running__ { false };
        std::mutex mu_cond__;
        std::condition_variable cond__;
    public:
        SF_SINGLE_TON(sf_eventloop);
        void exec();
        void wake();
        void quit();
    };

}
