
#pragma once
#include "co_mutex.h"
#include "json.h"
#include "object_msg_queue.h"
#include "random.h"
#include <bits/stdc++.h>

#define sf_singal(name, ...)                                                          \
public:                                                                               \
    co_recursive_mutex __mu_##name##_signal__;                                        \
    std::vector<std::tuple<std::function<void(__VA_ARGS__)>, bool, int>>              \
        __##name##_signal_func_vec__;                                                 \
    template <typename... __SF_OBJECT_ARGS__>                                         \
    void name(__SF_OBJECT_ARGS__&&... args)                                           \
    {                                                                                 \
        std::lock_guard<co_recursive_mutex> lck(__mu_##name##_signal__);              \
        for (auto& p : __##name##_signal_func_vec__)                                  \
        {                                                                             \
            if (std::get<1>(p))                                                       \
            {                                                                         \
                std::get<0>(p)(std::forward<__SF_OBJECT_ARGS__>(args)...);            \
            }                                                                         \
            else                                                                      \
            {                                                                         \
                auto bind_obj = std::bind(std::get<0>(p),                             \
                                          std::forward<__SF_OBJECT_ARGS__>(args)...); \
                __p_msg_queue__->add_msg(std::pair(this, bind_obj));                  \
            }                                                                         \
        }                                                                             \
    }
#define sf_bind(objptr, name, ...)                                    \
    (objptr)->__bind_helper__((objptr)->__mu_##name##_signal__,       \
                              (objptr)->__##name##_signal_func_vec__, \
                              __VA_ARGS__)
#define sf_unbind(objptr, name, bind_id)  \
    (objptr)->__unbind_helper__(          \
        (objptr)->__mu_##name##_signal__, \
        (objptr)->__##name##_signal_func_vec__, bind_id);
namespace skyfire
{
class object
{
public:
    template <typename _VectorType, typename _FuncType>
    int __bind_helper__(co_recursive_mutex& mu, _VectorType& vec,
                        _FuncType func, bool single_thread = true);
    template <typename _VectorType>
    void __unbind_helper__(co_recursive_mutex& mu, _VectorType& vec,
                           int bind_id);
    virtual ~object();

protected:
    std::shared_ptr<object_msg_queue> __p_msg_queue__ = object_msg_queue::instance();
};

// impl

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
