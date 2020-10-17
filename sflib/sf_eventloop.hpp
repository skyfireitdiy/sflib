
/**
* @version 1.0.0
* @author skyfire
* @file sf_eventloop.hpp
*/

/*
 * eventloop 事件循环
 */

#pragma once

#include "sf_eventloop.h"

namespace skyfire {
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
    while (true) {
        if (running__ == false) {
            break;
        }
        if (__p_msg_queue__->empty()) {
            __p_msg_queue__->wait_new_msg();
        }
        if (!__p_msg_queue__->empty()) {
            auto fp = __p_msg_queue__->take_msg();
            if (!fp) {
                continue;
            } else {
                fp->second();
            }
        }
    }
}

} // namespace skyfire
