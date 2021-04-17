
#pragma once
#include "eventloop.h"
#include "object.h"
#include "random.h"
namespace skyfire
{
template <typename _VectorType, typename _FuncType>
int object::__bind_helper__(co_recursive_mutex& mu, _VectorType& vec,
                            _FuncType func, bool single_thread)
{
    std::lock_guard<co_recursive_mutex> lck(mu);
    int                                 bind_id = random::instance()->rand_int(0, INT_MAX);
    while (std::find_if(vec.begin(), vec.end(), [=](auto p) {
               return std::get<2>(p) == bind_id;
           })
           != vec.end())
    {
        bind_id = random::instance()->rand_int(0, INT_MAX);
    }
    vec.push_back(std::make_tuple(std::function(func), single_thread, bind_id));
    return bind_id;
}
inline object::~object()
{
    __p_msg_queue__->remove_msg([this](std::pair<void*, std::function<void()>> d) -> bool {
        return d.first == this;
    });
}
template <typename _VectorType>
void object::__unbind_helper__(co_recursive_mutex& mu,
                               _VectorType& vec, int bind_id)
{
    std::lock_guard<co_recursive_mutex> lck(mu);
    vec.erase(std::remove_if(vec.begin(), vec.end(),
                             [=](auto p) { return std::get<2>(p) == bind_id; }),
              vec.end());
}
} // namespace skyfire
