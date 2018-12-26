
/**
* @version 1.0.0
* @author skyfire
* @mail skyfireitdiy@hotmail.com
* @see http://github.com/skyfireitdiy/sflib
* @file sf_object.h

* sflib第一版本发布
* 版本号1.0.0
* 发布日期：2018-10-22
*/

/*
 * sf_object 基础对象
 * 提供了信号---槽的通信机制、AOP编程
 */
#pragma once
#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCUnusedMacroInspection"


#include "sf_stdc++.h"
#include "sf_msg_queue.hpp"
#include "sf_json.hpp"


/*
 * SF_REG_SIGNAL 注册信号的宏
 */
#define SF_REG_SIGNAL(name, ...)                                                                                         \
public:                                                                                                                \
std::recursive_mutex __mu_##name##_signal_;                                                                             \
std::vector<std::tuple<std::function<void(__VA_ARGS__)>, bool, int>>__##name##_signal_func_vec__;                     \
template<typename...__SF_OBJECT_ARGS__>                                                                              \
void name(__SF_OBJECT_ARGS__&&... args) {                                                                              \
    std::lock_guard<std::recursive_mutex> lck(__mu_##name##_signal_);                                                   \
    for (auto &p :__##name##_signal_func_vec__)                                                                        \
    {                                                                                                                   \
        if (std::get<1>(p))                                                                                             \
        {                                                                                                               \
            std::get<0>(p)(std::forward<__SF_OBJECT_ARGS__>(args)...);                                                  \
        }                                                                                                               \
        else                                                                                                            \
        {                                                                                                               \
            auto bind_obj = std::bind(std::get<0>(p),                                                                   \
                                         std::forward<__SF_OBJECT_ARGS__>(args)...);                                    \
            __p_msg_queue__->add_msg(this, bind_obj);                                                                  \
                                                                                                                        \
        }                                                                                                               \
    }                                                                                                                   \
}                                                                                                                       \


/*
 * SF_REG_AOP 注册AOP的宏
 */
#define SF_REG_AOP(name, ...)                                                                                           \
public:                                                                                                                 \
    std::recursive_mutex __mu_##name##_aop__;                                                                            \
    std::vector<std::tuple<std::function<void(__VA_ARGS__)>,int>> __##name##_aop_before_func_vec__;                     \
    std::vector<std::tuple<std::function<void()>,int>> __##name##_aop_after_func_vec__;                                 \
    template<typename...__SF_OBJECT_ARGS__>                                                                            \
    decltype(auto) aop_##name(__SF_OBJECT_ARGS__&& ... args)                                                            \
    {                                                                                                                   \
        std::lock_guard<std::recursive_mutex> lck(__mu_##name##_aop__);                                                  \
        for(auto &p :  __##name##_aop_before_func_vec__)                                                                \
        {                                                                                                               \
            std::get<0>(p)(std::forward<__SF_OBJECT_ARGS__>(args)...);                                                  \
        }                                                                                                               \
        if constexpr (std::is_same<decltype(name(std::forward<__SF_OBJECT_ARGS__>(args)...)),void>::value)           \
        {                                                                                                               \
            name(std::forward<__SF_OBJECT_ARGS__>(args)...);                                                            \
            for(auto &p :  __##name##_aop_after_func_vec__)                                                             \
            {                                                                                                           \
                std::get<0>(p)();                                                                                       \
            }                                                                                                           \
        }                                                                                                               \
        else                                                                                                            \
        {                                                                                                               \
            decltype(auto) ret = name(std::forward<__SF_OBJECT_ARGS__>(args)...);                                      \
            for(auto &p :  __##name##_aop_after_func_vec__)                                                             \
            {                                                                                                           \
                std::get<0>(p)();                                                                                       \
            }                                                                                                           \
            return ret;                                                                                                 \
        }                                                                                                               \
    }                                                                                                                   \

/*
 * sf_aop_before_bind AOP调用前注册
 */
#define sf_aop_before_bind(objptr, name, func)                                                                          \
(objptr)->__sf_aop_before_add_helper((objptr)->__mu_##name##_aop__,(objptr)->__##name##_aop_before_func_vec__,func)     \

/*
 * sf_aop_after_bind AOP调用后注册
 */
#define sf_aop_after_bind(objptr, name, func)                                                                           \
(objptr)->__sf_aop_after_add_helper((objptr)->__mu_##name##_aop__,(objptr)->__##name##_aop_after_func_vec__,func)       \

/*
 * sf_aop_before_unbind AOP调用前反注册
 */
#define sf_aop_before_unbind(objptr, name, bind_id)                                                                       \
(objptr)->__sf_aop_unbind_helper((objptr)->__mu_##name##_aop__,(objptr)->__##name##_aop_before_func_vec__,bind_id);     \

/*
 * sf_aop_after_unbind AOP调用后反注册
 */
#define sf_aop_after_unbind(objptr, name, bind_id)                                                                        \
(objptr)->__sf_aop_unbind_helper((objptr)->__mu_##name##_aop__,(objptr)->__##name##_aop_after_func_vec__,bind_id);      \

/*
 * sf_bind_signal 信号绑定
 */
#define sf_bind_signal(objptr, name, func, mul_thread)                                                                     \
(objptr)->__sf_bind_helper((objptr)->__mu_##name##_signal_,(objptr)->__##name##_signal_func_vec__,func,mul_thread)      \


/*
 * sf_unbind_signal 信号解绑
 */
#define sf_unbind_signal(objptr, name, bind_id)                                                                           \
(objptr)->__sf_signal_unbind_helper((objptr)->__mu_##name##_signal_,(objptr)->__##name##_signal_func_vec__,bind_id);    \

#define _CONSTR(a, b) a##b
#define CONSTR(a, b) _CONSTR(a,b)


#define _SF_MAKE_BASE_JSON(name) std::function<skyfire::sf_json()>([=]()->skyfire::sf_json{                                             \
    skyfire::sf_json js;                                                                                                         \
    js[#name] = name;                                                                                                   \
    return js;                                                                                                          \
})                                                                                                                     \


#define _SF_MAKE_VALUE_JSON(name) std::function<skyfire::sf_json()>([=]()->skyfire::sf_json{                                               \
    skyfire::sf_json js;                                                                                                         \
    js[#name] = name.to_json();                                                                                         \
    return js;                                                                                                          \
})                                                                                                                     \



#define _SF_MAKE_POINTER_JSON(name) std::function<skyfire::sf_json()>([=]()->skyfire::sf_json{                                               \
    skyfire::sf_json js;                                                                                                         \
    js[#name] = name->to_json();                                                                                         \
    return js;                                                                                                          \
})                                                                                                                     \


#define _SF_MAKE_CONTAINER_BASE_JSON(name) std::function<skyfire::sf_json()>([=]()->skyfire::sf_json{                                             \
    skyfire::sf_json js;                                                                                                         \
    js[#name] = skyfire::sf_json();                                                                                                   \
    js[#name].convert_to_array();\
    for(auto &p:name){\
        js[#name].append(skyfire::sf_json(p));\
    }\
    return js;                                                                                                          \
})

#define _SF_MAKE_CONTAINER_VALUE_JSON(name) std::function<skyfire::sf_json()>([=]()->skyfire::sf_json{                                             \
    skyfire::sf_json js;                                                                                                         \
    js[#name] = skyfire::sf_json();                                                                                                   \
    js[#name].convert_to_array();\
    for(auto &p:name){\
        js[#name].append(p.to_json());\
    }\
    return js;                                                                                                          \
})



#define _SF_MAKE_CONTAINER_POINTER_JSON(name) std::function<skyfire::sf_json()>([=]()->skyfire::sf_json{                                             \
    skyfire::sf_json js;                                                                                                         \
    js[#name] = skyfire::sf_json();                                                                                                   \
    js[#name].convert_to_array();\
    for(auto &p:name){\
        js[#name].append(p->to_json());\
    }\
    return js;                                                                                                          \
})


#define _SF_MAKE_ASSOCIATED_CONTAINER_BASE_BASE_JSON(name) std::function<skyfire::sf_json()>([=]()->skyfire::sf_json{                   \
    skyfire::sf_json js;                                                                                                         \
    js[#name] = skyfire::sf_json();                                                                                                   \
    js[#name].convert_to_array();\
    for(auto &p:name){\
        skyfire::sf_json tmp_js;\
        tmp_js.convert_to_object();\
        tmp_js["key"] = p.first;\
        tmp_js["value"] = p.second;\
        js[#name].append(tmp_js);\
    }\
    return js;                                                                                                          \
})



#define _SF_MAKE_ASSOCIATED_CONTAINER_BASE_VALUE_JSON(name) std::function<skyfire::sf_json()>([=]()->skyfire::sf_json{\
    skyfire::sf_json js;\
    js[#name].convert_to_array();\
    for(auto &p:name){\
        skyfire::sf_json t;\
        t.convert_to_object();\
        t["key"] = p.first;\
        t["value"] = p.second.to_json();\
        js[#name].append(t);\
    }\
    return js;\
})\



#define _SF_MAKE_ASSOCIATED_CONTAINER_BASE_POINTER_JSON(name) std::function<skyfire::sf_json()>([=]()->skyfire::sf_json{\
    skyfire::sf_json js;\
    js[#name].convert_to_array();\
    for(auto &p:name){\
        skyfire::sf_json t;\
        t.convert_to_object();\
        t["key"] = p.first;\
        t["value"] = p.second->to_json();\
        js[#name].append(t);\
    }\
    return js;\
})\


#define _SF_MAKE_ASSOCIATED_CONTAINER_VALUE_BASE_JSON(name) std::function<skyfire::sf_json()>([=]()->skyfire::sf_json{\
    skyfire::sf_json js;\
    js[#name].convert_to_array();\
    for(auto &p:name){\
        skyfire::sf_json t;\
        t.convert_to_object();\
        t["key"] = p.first.to_json();\
        t["value"] = p.second;\
        js[#name].append(t);\
    }\
    return js;\
})\


#define _SF_MAKE_associated_container_base_base_JSON(name) std::function<skyfire::sf_json()>([=]()->skyfire::sf_json{\
    skyfire::sf_json js;\
    js[#name].convert_to_array();\
    for(auto &p:name){\
        skyfire::sf_json t;\
        t.convert_to_object();\
        t["key"] = p.first.to_json();\
        t["value"] = p.second.to_json();\
        js[#name].append(t);\
    }\
    return js;\
})\


#define _SF_MAKE_associated_container_base_pointer_JSON(name) std::function<skyfire::sf_json()>([=]()->skyfire::sf_json{\
    skyfire::sf_json js;\
    js[#name].convert_to_array();\
    for(auto &p:name){\
        skyfire::sf_json t;\
        t.convert_to_object();\
        t["key"] = p.first.to_json();\
        t["value"] = p.second->to_json();\
        js[#name].append(t);\
    }\
    return js;\
})\


#define _SF_MAKE_ASSOCIATED_CONTAINER_POINTER_BASE_JSON(name) std::function<skyfire::sf_json()>([=]()->skyfire::sf_json{\
    skyfire::sf_json js;\
    js[#name].convert_to_array();\
    for(auto &p:name){\
        skyfire::sf_json t;\
        t.convert_to_object();\
        t["key"] = p.first->to_json();\
        t["value"] = p.second;\
        js[#name].append(t);\
    }\
    return js;\
})\


#define _SF_MAKE_associated_container_pointer_base_JSON(name) std::function<skyfire::sf_json()>([=]()->skyfire::sf_json{\
    skyfire::sf_json js;\
    js[#name].convert_to_array();\
    for(auto &p:name){\
        skyfire::sf_json t;\
        t.convert_to_object();\
        t["key"] = p.first->to_json();\
        t["value"] = p.second.to_json();\
        js[#name].append(t);\
    }\
    return js;\
})\



#define _SF_MAKE_ASSOCIATED_CONTAINER_POINTER_POINTER_JSON(name) std::function<skyfire::sf_json()>([=]()->skyfire::sf_json{\
    skyfire::sf_json js;\
    js[#name].convert_to_array();\
    for(auto &p:name){\
        skyfire::sf_json t;\
        t.convert_to_object();\
        t["key"] = p.first->to_json();\
        t["value"] = p.second->to_json();\
        js[#name].append(t);\
    }\
    return js;\
})\



#define _SF_GETTER(type, name) type get_##name() const {                                                                  \
    return name;                                                                                                        \
}                                                                                                                       \


#define _SF_SETTER(type, name) void set_##name(const type& t){                                                            \
    this->name = t;                                                                                                      \
}                                                                                                                       \


#define SF_GS(type,name) _SF_GETTER(type,name) _SF_SETTER(type,name)


#define SF_GS_EXT(container,key_type,value_type, name) container<key_type,value_type> get_##name() const {\
    return name;\
}\
void set_##name(const container<key_type,value_type>& t){\
    this->name = t;\
}\



#define sf_class(x) namespace{\
                        skyfire::sf_object_meta_run CONSTR(class_meta_,__LINE__){\
                            []{\
                                skyfire::sf_object_global_meta_info::get_instance()->add_meta<x>(#x);\
                            }\
                        };\
                    }


#define sf_reg_class(x) skyfire::sf_object_meta_run CONSTR(class_reg_meta_,__LINE__){\
                            [=](){\
                                __class_name__ = #x;\
                            }\
                        };\


#define m_base(type, name) \
    private:\
        type name;\
        skyfire::sf_object_meta_run CONSTR(mem_meta_,__LINE__){\
            [=](){ \
                mem_value_type__[#name] = sf_object::__mem_value_type_t__ ::value;\
                member_base_callback__[#name]= \
                    [=](std::any value){\
                        type tmp_value = static_cast<type>(std::any_cast<skyfire::sf_json>(value));\
                        name = tmp_value;\
                    };\
                to_json_callback__[#name] = _SF_MAKE_BASE_JSON(name);\
            }\
        };\
    public: \
        SF_GS(type,name)



#define m_value(type, name)\
    private:\
        type name;\
        skyfire::sf_object_meta_run CONSTR(mem_meta_,__LINE__){ \
            [=](){\
                mem_value_type__[#name] = sf_object::__mem_value_type_t__ ::ref;\
                member_value_callback__[#name] = \
                    [=](std::shared_ptr<sf_object> value){\
                        std::shared_ptr<type> tmp_value = std::dynamic_pointer_cast<type>(value);\
                        name = *tmp_value;\
                    };\
                to_json_callback__[#name]  =_SF_MAKE_VALUE_JSON(name);\
            }\
        };\
    public: \
        SF_GS(type, name)\





#define m_pointer(type, name) \
    private:\
        std::shared_ptr<type> name;\
        skyfire::sf_object_meta_run CONSTR(mem_meta_,__LINE__){\
            [=](){\
                mem_value_type__[#name] = sf_object::__mem_value_type_t__ ::pointer;\
                member_pointer_callback__[#name] = \
                    [=](std::shared_ptr<sf_object> value){\
                        std::shared_ptr<type> tmp_value = std::dynamic_pointer_cast<type>(value);\
                        name = tmp_value;\
                    };\
                to_json_callback__[#name]  = _SF_MAKE_POINTER_JSON(name);\
            }\
        };\
    public: \
        SF_GS(std::shared_ptr<type>, name)\




#define mc_base(container_type, type, name) \
    private:\
        container_type<type> name;\
        skyfire::sf_object_meta_run CONSTR(mem_meta_,__LINE__){\
            [=](){ \
                mem_value_type__[#name] = sf_object::__mem_value_type_t__ ::container_value;\
                member_container_base_callback__[#name]= \
                    [=](std::any value){\
                        skyfire::sf_json tmp_json = std::any_cast<skyfire::sf_json>(value);\
                        int sz = tmp_json.size();\
                        std::vector<type> tmp_vec(sz);\
                        for(int i=0;i<sz;++i){\
                            tmp_vec[i] = static_cast<type>(tmp_json[i]);\
                        }\
                        name = container_type<type>(tmp_vec.begin(),tmp_vec.end());\
                    };\
                to_json_callback__[#name] = _SF_MAKE_CONTAINER_BASE_JSON(name);\
            }\
        };\
    public: \
        SF_GS(container_type<type>,name)



#define mc_value(container_type, type, name) \
    private:\
        container_type<type> name;\
        skyfire::sf_object_meta_run CONSTR(mem_meta_,__LINE__){\
            [=](){ \
                mem_value_type__[#name] = sf_object::__mem_value_type_t__ ::container_ref;\
                member_container_value_callback__[#name]= \
                    [=](std::vector<std::shared_ptr<sf_object>> value){\
                        int sz = value.size();\
                        std::vector<type> tmp_vec(sz);\
                        for(int i=0;i<sz;++i){\
                            tmp_vec[i]=(*std::dynamic_pointer_cast<type>(value[i]));\
                        }\
                        name = container_type<type>(tmp_vec.begin(),tmp_vec.end());\
                    };\
                to_json_callback__[#name] = _SF_MAKE_CONTAINER_VALUE_JSON(name);\
            }\
        };\
    public: \
        SF_GS(container_type<type>,name)



#define mc_pointer(container_type, type, name) \
    private:\
        container_type<std::shared_ptr<type>> name;\
        skyfire::sf_object_meta_run CONSTR(mem_meta_,__LINE__){\
            [=](){ \
                mem_value_type__[#name] = sf_object::__mem_value_type_t__ ::container_pointer;\
                member_container_pointer_callback__[#name]= \
                    [=](std::vector<std::shared_ptr<sf_object>> value){\
                        int sz = value.size();\
                        std::vector<std::shared_ptr<type>> tmp_vec(sz);\
                        for(int i=0;i<sz;++i){\
                            tmp_vec[i]=(std::dynamic_pointer_cast<type>(value[i]));\
                        }\
                        name = container_type<std::shared_ptr<type>>(tmp_vec.begin(),tmp_vec.end());\
                    };\
                to_json_callback__[#name] = _SF_MAKE_CONTAINER_POINTER_JSON(name);\
            }\
        };\
    public: \
        SF_GS(container_type<std::shared_ptr<type>>,name)


#define mac_bb(container_type, key_type, value_type, name) \
    private:\
        container_type<key_type, value_type> name;\
        skyfire::sf_object_meta_run CONSTR(mem_meta_,__LINE__){\
            [=](){ \
                mem_value_type__[#name] = sf_object::__mem_value_type_t__ ::associated_container_base_base;\
                member_associated_container_base_base_callback__[#name]= \
                    [=](std::any value){\
                        auto js = std::any_cast<skyfire::sf_json>(value);\
                        int sz = js.size();\
                        std::vector<std::pair<key_type,value_type>> tmp_vec(sz);\
                        for(int i=0;i<sz;++i){\
                            auto tmp_pair = std::make_pair(static_cast<key_type>(js[i]["key"]),static_cast<value_type>(js[i]["value"]));\
                            tmp_vec[i]=tmp_pair;\
                        }\
                        name = container_type<key_type, value_type>(tmp_vec.begin(),tmp_vec.end());\
                    };\
                to_json_callback__[#name] = _SF_MAKE_ASSOCIATED_CONTAINER_BASE_BASE_JSON(name);\
            }\
        };\
    public: \
        SF_GS_EXT(container_type,key_type, value_type,name)




#define mac_bv(container_type, key_type, value_type, name) \
    private:\
        container_type<key_type, value_type> name;\
        skyfire::sf_object_meta_run CONSTR(mem_meta_,__LINE__){\
            [=](){ \
                mem_value_type__[#name] = sf_object::__mem_value_type_t__ ::associated_container_base_value;\
                member_associated_container_base_value_callback__[#name]= \
                    [=](std::vector<std::pair<std::any,std::shared_ptr<sf_object>>> value){\
                        std::vector<std::pair<key_type,value_type>> tmp_vec;\
                        for(auto &p:value){\
                            auto t = std::make_pair(static_cast<key_type>(std::any_cast<skyfire::sf_json>(p.first)),*std::dynamic_pointer_cast<value_type>(p.second));\
                            tmp_vec.push_back(t);\
                        }\
                        name = container_type<key_type, value_type>(tmp_vec.begin(),tmp_vec.end());\
                    };\
                to_json_callback__[#name] = _SF_MAKE_ASSOCIATED_CONTAINER_BASE_VALUE_JSON(name);\
            }\
        };\
    public: \
        SF_GS_EXT(container_type,key_type, value_type,name)


#define mac_bp(container_type, key_type, value_type, name) \
    private:\
        container_type<key_type, std::shared_ptr< value_type>> name;\
        skyfire::sf_object_meta_run CONSTR(mem_meta_,__LINE__){\
            [=](){ \
                mem_value_type__[#name] = sf_object::__mem_value_type_t__ ::associated_container_base_pointer;\
                member_associated_container_base_pointer_callback__[#name]= \
                    [=](std::vector<std::pair<std::any,std::shared_ptr<sf_object>>> value){\
                        std::vector<std::pair<key_type,std::shared_ptr<value_type>>> tmp_vec;\
                        for(auto &p:value){\
                            auto t = std::make_pair(static_cast<key_type>(std::any_cast<skyfire::sf_json>(p.first)),std::dynamic_pointer_cast<value_type>(p.second));\
                            tmp_vec.push_back(t);\
                        }\
                        name = container_type<key_type, std::shared_ptr<value_type>>(tmp_vec.begin(),tmp_vec.end());\
                    };\
                to_json_callback__[#name] = _SF_MAKE_ASSOCIATED_CONTAINER_BASE_POINTER_JSON(name);\
            }\
        };\
    public: \
        SF_GS_EXT(container_type,key_type, std::shared_ptr<value_type>,name)



#define mac_vb(container_type, key_type, value_type, name) \
    private:\
        container_type<key_type,value_type> name;\
        skyfire::sf_object_meta_run CONSTR(mem_meta_,__LINE__){\
            [=](){ \
                mem_value_type__[#name] = sf_object::__mem_value_type_t__ ::associated_container_value_base;\
                member_associated_container_value_base_callback__[#name]= \
                    [=](std::vector<std::pair<std::shared_ptr<sf_object>,std::any>> value){\
                        std::vector<std::pair<key_type,value_type>> tmp_vec;\
                        for(auto &p:value){\
                            auto t = std::make_pair(*std::dynamic_pointer_cast<key_type>(p.first),static_cast<value_type>(std::any_cast<skyfire::sf_json>(p.second)));\
                            tmp_vec.push_back(t);\
                        }\
                        name = container_type<key_type, value_type>(tmp_vec.begin(),tmp_vec.end());\
                    };\
                to_json_callback__[#name] = _SF_MAKE_ASSOCIATED_CONTAINER_VALUE_BASE_JSON(name);\
            }\
        };\
    public: \
        SF_GS_EXT(container_type,key_type, value_type,name)


#define mac_vv(container_type, key_type, value_type, name) \
    private:\
        container_type<key_type,value_type> name;\
        skyfire::sf_object_meta_run CONSTR(mem_meta_,__LINE__){\
            [=](){ \
                mem_value_type__[#name] = sf_object::__mem_value_type_t__ ::associated_container_value_value;\
                member_associated_container_value_value_callback__[#name]= \
                    [=](std::vector<std::pair<std::shared_ptr<sf_object>,std::shared_ptr<sf_object>>> value){\
                        std::vector<std::pair<key_type,value_type>> tmp_vec;\
                        for(auto &p:value){\
                            auto t = std::make_pair(*std::dynamic_pointer_cast<key_type>(p.first),*std::dynamic_pointer_cast<value_type>(p.second));\
                            tmp_vec.push_back(t);\
                        }\
                        name = container_type<key_type, value_type>(tmp_vec.begin(),tmp_vec.end());\
                    };\
                to_json_callback__[#name] = _SF_MAKE_associated_container_base_base_JSON(name);\
            }\
        };\
    public: \
        SF_GS_EXT(container_type,key_type, value_type,name)


#define mac_vp(container_type, key_type, value_type, name) \
    private:\
        container_type<key_type,std::shared_ptr<value_type>> name;\
        skyfire::sf_object_meta_run CONSTR(mem_meta_,__LINE__){\
            [=](){ \
                mem_value_type__[#name] = sf_object::__mem_value_type_t__ ::associated_container_value_pointer;\
                member_associated_container_value_pointer_callback__[#name]= \
                    [=](std::vector<std::pair<std::shared_ptr<sf_object>,std::shared_ptr<sf_object>>> value){\
                        std::vector<std::pair<key_type,std::shared_ptr<value_type>>> tmp_vec;\
                        for(auto &p:value){\
                            auto t = std::make_pair(*std::dynamic_pointer_cast<key_type>(p.first),std::dynamic_pointer_cast<value_type>(p.second));\
                            tmp_vec.push_back(t);\
                        }\
                        name = container_type<key_type, std::shared_ptr<value_type>>(tmp_vec.begin(),tmp_vec.end());\
                    };\
                to_json_callback__[#name] = _SF_MAKE_associated_container_base_pointer_JSON(name);\
            }\
        };\
    public: \
        SF_GS_EXT(container_type,key_type, std::shared_ptr<value_type>,name)



#define mac_pb(container_type, key_type, value_type, name) \
    private:\
        container_type<std::shared_ptr<key_type>,value_type> name;\
        skyfire::sf_object_meta_run CONSTR(mem_meta_,__LINE__){\
            [=](){ \
                mem_value_type__[#name] = sf_object::__mem_value_type_t__ ::associated_container_pointer_base;\
                member_associated_container_pointer_base_callback__[#name]= \
                    [=](std::vector<std::pair<std::shared_ptr<sf_object>,std::any>> value){\
                        std::vector<std::pair<std::shared_ptr<key_type>,value_type>> tmp_vec;\
                        for(auto &p:value){\
                            auto t = std::make_pair(std::dynamic_pointer_cast<key_type>(p.first),static_cast<value_type>(std::any_cast<skyfire::sf_json>(p.second)));\
                            tmp_vec.push_back(t);\
                        }\
                        name = container_type<std::shared_ptr<key_type>, value_type>(tmp_vec.begin(),tmp_vec.end());\
                    };\
                to_json_callback__[#name] = _SF_MAKE_ASSOCIATED_CONTAINER_POINTER_BASE_JSON(name);\
            }\
        };\
    public: \
        SF_GS_EXT(container_type,std::shared_ptr<key_type>, value_type,name)



#define mac_pv(container_type, key_type, value_type, name) \
    private:\
        container_type<std::shared_ptr<key_type>,value_type> name;\
        skyfire::sf_object_meta_run CONSTR(mem_meta_,__LINE__){\
            [=](){ \
                mem_value_type__[#name] = sf_object::__mem_value_type_t__ ::associated_container_pointer_value;\
                member_associated_container_pointer_value_callback__[#name]= \
                    [=](std::vector<std::pair<std::shared_ptr<sf_object>,std::shared_ptr<sf_object>>> value){\
                        std::vector<std::pair<std::shared_ptr<key_type>,value_type>> tmp_vec;\
                        for(auto &p:value){\
                            auto t = std::make_pair(std::dynamic_pointer_cast<key_type>(p.first),*std::dynamic_pointer_cast<key_type>(p.second));\
                            tmp_vec.push_back(t);\
                        }\
                        name = container_type<std::shared_ptr<key_type>, value_type>(tmp_vec.begin(),tmp_vec.end());\
                    };\
                to_json_callback__[#name] = _SF_MAKE_associated_container_pointer_base_JSON(name);\
            }\
        };\
    public: \
        SF_GS_EXT(container_type,std::shared_ptr<key_type>, value_type,name)


#define mac_pp(container_type, key_type, value_type, name) \
    private:\
        container_type<std::shared_ptr<key_type>,std::shared_ptr<value_type>> name;\
        skyfire::sf_object_meta_run CONSTR(mem_meta_,__LINE__){\
            [=](){ \
                mem_value_type__[#name] = sf_object::__mem_value_type_t__ ::associated_container_pointer_pointer;\
                member_associated_container_pointer_pointer_callback__[#name]= \
                    [=](std::vector<std::pair<std::shared_ptr<sf_object>,std::shared_ptr<sf_object>>> value){\
                        std::vector<std::pair<std::shared_ptr<key_type>,std::shared_ptr<value_type>>> tmp_vec;\
                        for(auto &p:value){\
                            auto t = std::make_pair(std::dynamic_pointer_cast<key_type>(p.first),std::dynamic_pointer_cast<key_type>(p.second));\
                            tmp_vec.push_back(t);\
                        }\
                        name = container_type<std::shared_ptr<key_type>, std::shared_ptr<value_type>>(tmp_vec.begin(),tmp_vec.end());\
                    };\
                to_json_callback__[#name] = _SF_MAKE_ASSOCIATED_CONTAINER_POINTER_POINTER_JSON(name);\
            }\
        };\
    public: \
        SF_GS_EXT(container_type,std::shared_ptr<key_type>, std::shared_ptr<value_type>,name)



namespace skyfire {
    /**
     *  @brief 元对象
     */
    class sf_object {

    public:

        enum class __mem_value_type_t__ {
            value,
            ref,
            pointer,
            container_value,
            container_ref,
            container_pointer,
            associated_container_base_base,
            associated_container_base_value,
            associated_container_base_pointer,
            associated_container_value_base,
            associated_container_value_value,
            associated_container_value_pointer,
            associated_container_pointer_base,
            associated_container_pointer_value,
            associated_container_pointer_pointer,
            none
        };

        template<typename _VectorType, typename _FuncType>
        int __sf_bind_helper(std::recursive_mutex &mu, _VectorType &vec, _FuncType func, bool mul_thread);

        template<typename _VectorType>
        void __sf_signal_unbind_helper(std::recursive_mutex &mu, _VectorType &vec, int bind_id);

        template<typename _VectorType, typename _FuncType>
        int __sf_aop_before_add_helper(std::recursive_mutex &mu, _VectorType &vec, _FuncType func);

        template<typename _VectorType, typename _FuncType>
        int __sf_aop_after_add_helper(std::recursive_mutex &mu, _VectorType &vec, _FuncType func);


        template<typename _VectorType>
        void __sf_aop_unbind_helper(std::recursive_mutex &mu, _VectorType &vec, int bind_id);

        __mem_value_type_t__ __get_mem_value_type(const std::string &key);

        virtual skyfire::sf_json to_json() const;

        virtual ~sf_object();

        _SF_GETTER(std::string, __class_name__)
        _SF_GETTER(std::string, __object_id__)


    public:
        std::unordered_map<std::string, std::function<void(std::any)>> member_base_callback__;
        std::unordered_map<std::string, std::function<void(std::shared_ptr<sf_object>)>> member_value_callback__;
        std::unordered_map<std::string, std::function<void(std::shared_ptr<sf_object>)>> member_pointer_callback__;

        std::unordered_map<std::string, std::function<void(std::any)>> member_container_base_callback__;
        std::unordered_map<std::string, std::function<void(std::vector<std::shared_ptr<sf_object>>)>> member_container_value_callback__;
        std::unordered_map<std::string, std::function<void(std::vector<std::shared_ptr<sf_object>>)>> member_container_pointer_callback__;

        std::unordered_map<std::string, std::function<void(std::any)>> member_associated_container_base_base_callback__;
        std::unordered_map<std::string, std::function<void(std::vector<std::pair<std::any,std::shared_ptr<sf_object>>>)>> member_associated_container_base_value_callback__;
        std::unordered_map<std::string, std::function<void(std::vector<std::pair<std::any,std::shared_ptr<sf_object>>>)>> member_associated_container_base_pointer_callback__;

        std::unordered_map<std::string, std::function<void(std::vector< std::pair<std::shared_ptr<sf_object>,std::any>>)>> member_associated_container_value_base_callback__;
        std::unordered_map<std::string, std::function<void(std::vector< std::pair<std::shared_ptr<sf_object>,std::shared_ptr<sf_object>>>)>> member_associated_container_value_value_callback__;
        std::unordered_map<std::string, std::function<void(std::vector< std::pair<std::shared_ptr<sf_object>,std::shared_ptr<sf_object>>>)>> member_associated_container_value_pointer_callback__;

        std::unordered_map<std::string, std::function<void(std::vector< std::pair<std::shared_ptr<sf_object>,std::any>>)>> member_associated_container_pointer_base_callback__;
        std::unordered_map<std::string, std::function<void(std::vector< std::pair<std::shared_ptr<sf_object>,std::shared_ptr<sf_object>>>)>> member_associated_container_pointer_value_callback__;
        std::unordered_map<std::string, std::function<void(std::vector< std::pair<std::shared_ptr<sf_object>,std::shared_ptr<sf_object>>>)>> member_associated_container_pointer_pointer_callback__;

        std::unordered_map<std::string, std::function<skyfire::sf_json()>> to_json_callback__;
        std::unordered_map<std::string, __mem_value_type_t__> mem_value_type__;

        friend class sf_object_global_meta_info;

        std::string __class_name__;
        std::string __object_id__;

    protected:
        sf_msg_queue *__p_msg_queue__ = sf_msg_queue::get_instance();
    };

}

#pragma clang diagnostic pop