
#include "eventloop.h"
namespace skyfire
{
void eventloop::quit()
{
    running__ = false;
    wake();
}
void eventloop::wake() const { __p_msg_queue__->add_empty_msg(); }
void eventloop::clear() const { __p_msg_queue__->clear(); }
void eventloop::exec()
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
