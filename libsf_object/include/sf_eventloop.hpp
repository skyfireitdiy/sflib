/*
 * sf_eventloop 事件循环
 */

#pragma once

#include "sf_eventloop.h"

namespace skyfire
{

    inline sf_eventloop::sf_eventloop() {

    }

    inline void sf_eventloop::quit() {
        running__ = false;
        wake();
    }

    inline void sf_eventloop::wake() {
        __p_msg_queue__->add_empty_msg();
    }

    inline void sf_eventloop::clear() {
        __p_msg_queue__->clear();
    }

    inline void sf_eventloop::exec() {
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


}
