#include "sf_eventloop.h"

namespace skyfire
{
    void sf_eventloop::exec()
    {
        if(running__)
        {
            return;
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

    void sf_eventloop::wake()
    {
        cond__.notify_one();
    }

    void sf_eventloop::quit()
    {
        running__ = false;
        wake();
    }
}