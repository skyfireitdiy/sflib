
/**
* @version 1.0.0
* @author skyfire
* @mail skyfireitdiy@hotmail.com
* @see http://github.com/skyfireitdiy/sflib
* @file sf_eventloop.hpp

* sflib第一版本发布
* 版本号1.0.0
* 发布日期：2018-10-22
*/

/*
 * sf_eventloop 事件循环
 */

#pragma once

#include "sf_eventloop.h"

namespace skyfire {
inline void sf_eventloop::quit() {
    running__ = false;
    wake();
}

inline void sf_eventloop::wake() const { __p_msg_queue__->add_empty_msg(); }

inline void sf_eventloop::clear() const { __p_msg_queue__->clear(); }

inline void sf_eventloop::exec() {
    running__ = true;
    while (true) {
        if (running__ == false) {
            break;
        }
        if (__p_msg_queue__->empty()) {
            __p_msg_queue__->wait_msg();
        }
        if (!__p_msg_queue__->empty()) {
            auto func = __p_msg_queue__->take();
            if (!func) {
                continue;
            } else {
                func();
            }
        }
    }
}

}    // namespace skyfire
