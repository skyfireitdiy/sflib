#pragma once

#include <map>
#include <vector>
#include <functional>
#include "sf_empty_class.h"

#define sf_emit 
#define SF_REG_SIGNAL(name,...)                                             \
public:                                                                     \
std::vector<std::function<void(__VA_ARGS__)>> __##name##_func_vec__;        \
template<typename...ARGS>                                                   \
void name(ARGS... args)                                                     \
{                                                                           \
    for(auto &p : __##name##_func_vec__)                                    \
    {                                                                       \
        p(args...);                                                         \
    }                                                                       \
}                                                                           \


#define sf_bind_signal(objptr,name,func)                                    \
(objptr)->__sf_bind_helper((objptr)->__##name##_func_vec__,func)            \


namespace skyfire
{
    template<typename _BaseClass = sf_empty_class>
    class sf_object : public _BaseClass
    {
        public:
        template<typename _VectorType, typename _FuncType>
        void __sf_bind_helper(_VectorType &vec, _FuncType func)
        {
            vec.push_back(func);
        }
    };
}