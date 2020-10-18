#pragma once
#include "sf_msg_queue.hpp"
#include "sf_stdc++.h"

namespace skyfire {

/**
 * @brief object专用消息队列
 * 
 */
class object_msg_queue : public msg_queue<std::pair<void*, std::function<void()>>> {
public:
    SF_SINGLE_TON(object_msg_queue)
    object_msg_queue() = default;
};
}