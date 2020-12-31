#pragma once
#include "sf_msg_queue.hpp"
#include "sf_stdc++.h"
namespace skyfire
{
class object_msg_queue : public msg_queue<std::pair<void*, std::function<void()>>>
{
public:
    SF_SINGLE_TON(object_msg_queue)
    object_msg_queue() = default;
};
}