#pragma once
#include "core/sf_msg_queue.hpp"
#include "core/sf_stdc++.h"

namespace skyfire {
class sf_object_msg_queue : public sf_msg_queue<std::pair<void*, std::function<void()>>> {
public:
    SF_SINGLE_TON(sf_object_msg_queue)
    sf_object_msg_queue() = default;
};
}