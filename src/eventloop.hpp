
#pragma once
#include "eventloop.h"
namespace skyfire
{
inline void eventloop::quit()
{
    running__ = false;
    wake();
}
inline void eventloop::wake() const { __p_msg_queue__->add_empty_msg(); }
inline void eventloop::clear() const { __p_msg_queue__->clear(); }
inline void eventloop::exec()
{
    running__ = true;
    while (true)
    {
        if (running__ == false)
        {
            break;
        }
        __p_msg_queue__->wait_new_msg();
        auto fp = __p_msg_queue__->take_msg();
        if (fp == nullptr)
        {
            continue;
        }
        else
        {
            fp->second();
        }
    }
}
} // namespace skyfire
