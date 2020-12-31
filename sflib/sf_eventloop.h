
#pragma once
#include "sf_nocopy.h"
#include "sf_object_msg_queue.hpp"
#include "sf_stdc++.h"
namespace skyfire
{
class eventloop final : nocopy<>
{
private:
    object_msg_queue* __p_msg_queue__ = object_msg_queue::instance();
    std::atomic<int>  running__ { 0 };

public:
    eventloop() = default;
    void exec();
    void clear() const;
    void wake() const;
    void quit();
};
} // namespace skyfire
