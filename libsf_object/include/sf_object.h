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
std::vector<std::pair<std::function<void(__VA_ARGS__)>, bool>> __##name##_func_vec__;       \
template<typename...ARGS>                                                                   \
void name(ARGS... args) {                                                                   \
    for (auto &p : __##name##_func_vec__)                                                   \
    {                                                                                       \
        if (p.second)                                                                       \
        {                                                                                   \
            std::thread([=]() {                                                             \
                p.first(args...);                                                           \
            }).detach();                                                                    \
        }                                                                                   \
        else                                                                                \
        {                                                                                   \
            std::thread([=](){                                                              \
                __p_msg_queue__->add_msg(this, std::bind(p.first,args...));                 \
                __p_eventloop__->wake();                                                    \
            }).detach();                                                                    \
        }                                                                                   \
    }                                                                                       \
}                                                                                           \



#define sf_bind_signal(objptr,name,func,mul_thread)                                         \
(objptr)->__sf_bind_helper((objptr)->__##name##_func_vec__,func,mul_thread)                 \


namespace skyfire
{
    template<typename _BaseClass = sf_empty_class>
    class sf_object : public _BaseClass
    {
    public:
        template<typename _VectorType, typename _FuncType>
        void __sf_bind_helper(_VectorType &vec, _FuncType func, bool mul_thread)
        {
            vec.push_back({func, mul_thread});
        }

        virtual ~sf_object()
        {
            __p_msg_queue__->remove_msg(this);
        }

    protected:
        sf_msg_queue* __p_msg_queue__ = sf_msg_queue::get_instance();
        sf_eventloop* __p_eventloop__ = sf_eventloop::get_instance();
    };

}