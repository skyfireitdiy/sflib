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
        void exec(){
            if(running__)
            {
                while(true)
                {
                    if(running__ == false)
                    {
                        return;
                    }
                    std::unique_lock<std::mutex> lck(mu_cond__);
                    cond__.wait(lck);
                }
            }
            running__ = true;
            __p_msg_queue__->clear();
            while(true)
            {
                if(running__ == false)
                {
                    break;
                }
                if(__p_msg_queue__->empty())
                {
                    std::unique_lock<std::mutex> lck(mu_cond__);
                    cond__.wait(lck);
                }
                if(!__p_msg_queue__->empty())
                {
                    auto func = __p_msg_queue__->take();
                    if(!func)
                    {
                        running__ = false;
                        break;
                    }
                    else
                    {
                        func();
                    }
                }
            }
        }


        void wake()
        {
            cond__.notify_all();
        }


        void quit()
        {
            running__ = false;
            wake();
        }
    };

}
