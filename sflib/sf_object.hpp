
/**
* @version 1.0.0
* @author skyfire
* @file sf_object.hpp
*/

/*
 * sf_object 基础对象
 * 提供了信号---槽的通信机制、AOP编程
 */

#pragma once

#include "sf_eventloop.hpp"
#include "sf_object.h"
#include "sf_random.hpp"

namespace skyfire {

template <typename _VectorType, typename _FuncType>
int sf_object::__sf_bind_helper(std::recursive_mutex& mu, _VectorType& vec,
    _FuncType func, bool single_thread)
{
    std::lock_guard<std::recursive_mutex> lck(mu);
    int bind_id = sf_random::instance()->rand_int(0, INT_MAX);
    while (std::find_if(vec.begin(), vec.end(), [=](auto p) {
        return std::get<2>(p) == bind_id;
    }) != vec.end()) {
        bind_id = sf_random::instance()->rand_int(0, INT_MAX);
    }
    vec.push_back(std::make_tuple(std::function(func), single_thread, bind_id));
    return bind_id;
}

inline sf_object::~sf_object()
{
    __p_msg_queue__->remove_msg([this](std::pair<void*, std::function<void()>> d) -> bool {
        return d.first == this;
    });
}

template <typename _VectorType>
void sf_object::__sf_signal_unbind_helper(std::recursive_mutex& mu,
    _VectorType& vec, int bind_id)
{
    std::lock_guard<std::recursive_mutex> lck(mu);
    vec.erase(std::remove_if(vec.begin(), vec.end(),
                  [=](auto p) { return std::get<2>(p) == bind_id; }),
        vec.end());
}

} // namespace skyfire
