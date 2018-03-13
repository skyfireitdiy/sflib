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
        sf_eventloop(){

        }
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

        void clear()
        {
            __p_msg_queue__->clear();
        }

        void wake()
        {
            __p_msg_queue__->add_empty_msg();
        }


        void quit()
        {
            running__ = false;
            wake();
        }
    };


}
