
#pragma once
#include "sf_chan.h"
namespace skyfire
{
template <typename T>
chan<T>::chan(size_t buffer_size)
{
    max_size__ = buffer_size;
}
template <typename T>
void chan<T>::close()
{
    std::unique_lock<std::mutex> lck(mu__);
    closed__ = true;
    // FIXME deque没有clear成员了？
    // data__.clear();
    data__ = std::queue<T> {};
    if (max_size__ != 0)
    {
        cond__.notify_all();
    }
    else
    {
        cond_pop__.notify_all();
    }
}
}