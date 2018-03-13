#pragma once

#include <map>
#include <vector>
#include <thread>
#include <functional>
#include "sf_empty_class.h"
#include "sf_msg_queue.h"
#include "sf_eventloop.h"

#define SF_REG_SIGNAL(name,...)                                                             \
public:                                                                                     \
std::mutex __mu_##name##__;                                                                 \
std::vector<std::tuple<std::function<void(__VA_ARGS__)>, bool, int>> __##name##_func_vec__; \
template<typename...__SF_OBJECT_ARGS__>                                                     \
void name(__SF_OBJECT_ARGS__... args) {                                                     \
    std::lock_guard<std::mutex> lck(__mu_##name##__);                                       \
    for (auto &p : __##name##_func_vec__)                                                   \
    {                                                                                       \
        if (std::get<1>(p))                                                                 \
        {                                                                                   \
            std::thread([=]() {                                                             \
                std::get<0>(p)(args...);                                                    \
            }).detach();                                                                    \
        }                                                                                   \
        else                                                                                \
        {                                                                                   \
            std::thread([=](){                                                              \
                __p_msg_queue__->add_msg(this, std::bind(std::get<0>(p),args...));          \
            }).detach();                                                                    \
        }                                                                                   \
    }                                                                                       \
}                                                                                           \



#define sf_bind_signal(objptr,name,func,mul_thread)                                                                 \
(objptr)->__sf_bind_helper((objptr)->__mu_##name##__,(objptr)->__##name##_func_vec__,func,mul_thread)               \


#define sf_unbind_signal(objptr,name,bind_id)                                                                          \
(objptr)->__sf_unbind_helper((objptr)->__mu_##name##__,(objptr)->__##name##_func_vec__,bind_id);                       \


namespace skyfire
{
    template<typename _BaseClass = sf_empty_class>
    class sf_object : public _BaseClass
    {
    public:
        template<typename _VectorType, typename _FuncType>
        int __sf_bind_helper(std::mutex &mu,_VectorType &vec, _FuncType func, bool mul_thread)
        {
            std::lock_guard<std::mutex> lck(mu);
            int bind_id = rand();
            while(std::find_if(vec.begin(),vec.end(),[=](auto p){
                return std::get<2>(p) == bind_id;
            }) != vec.end())
            {
                bind_id = rand();
            }
            vec.push_back(std::make_tuple(func, mul_thread, bind_id));
            return bind_id;
        }

        template<typename _VectorType>
        void __sf_unbind_helper(std::mutex &mu,_VectorType &vec, int bind_id)
        {
            std::lock_guard<std::mutex> lck(mu);
            std::remove_if(vec.begin(),vec.end(),[=](auto p){
                return std::get<2>(p) == bind_id;
            });
        }

        virtual ~sf_object()
        {
            __p_msg_queue__->remove_msg(this);
        }

    protected:
        sf_msg_queue* __p_msg_queue__ = sf_msg_queue::get_instance();
    };

}