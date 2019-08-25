
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

#include "core/sf_msg_queue.hpp"
#include "sf_stdc++.h"
#include "tools/sf_json.hpp"

/*
 * SF_REG_SIGNAL 注册信号的宏
 */
#define SF_REG_SIGNAL(name, ...)                                           \
   public:                                                                 \
    std::recursive_mutex __mu_##name##_signal_;                            \
    std::vector<std::tuple<std::function<void(__VA_ARGS__)>, bool, int>>   \
        __##name##_signal_func_vec__;                                      \
    template <typename... __SF_OBJECT_ARGS__>                              \
    void name(__SF_OBJECT_ARGS__ &&... args) {                             \
        std::lock_guard<std::recursive_mutex> lck(__mu_##name##_signal_);  \
        for (auto &p : __##name##_signal_func_vec__) {                     \
            if (std::get<1>(p)) {                                          \
                std::get<0>(p)(std::forward<__SF_OBJECT_ARGS__>(args)...); \
            } else {                                                       \
                auto bind_obj =                                            \
                    std::bind(std::get<0>(p),                              \
                              std::forward<__SF_OBJECT_ARGS__>(args)...);  \
                __p_msg_queue__->add_msg(this, bind_obj);                  \
            }                                                              \
        }                                                                  \
    }

/*
 * SF_REG_AOP 注册AOP的宏
 */
#define SF_REG_AOP(name, ...)                                                 \
   public:                                                                    \
    std::recursive_mutex __mu_##name##_aop__;                                 \
    std::vector<std::tuple<std::function<void(__VA_ARGS__)>, int>>            \
        __##name##_aop_before_func_vec__;                                     \
    std::vector<std::tuple<std::function<void()>, int>>                       \
        __##name##_aop_after_func_vec__;                                      \
    template <typename... __SF_OBJECT_ARGS__>                                 \
    decltype(auto) aop_##name(__SF_OBJECT_ARGS__ &&... args) {                \
        std::lock_guard<std::recursive_mutex> lck(__mu_##name##_aop__);       \
        for (auto &p : __##name##_aop_before_func_vec__) {                    \
            std::get<0>(p)(std::forward<__SF_OBJECT_ARGS__>(args)...);        \
        }                                                                     \
        if constexpr (std::is_same<decltype(                                  \
                                       name(std::forward<__SF_OBJECT_ARGS__>( \
                                           args)...)),                        \
                                   void>::value) {                            \
            name(std::forward<__SF_OBJECT_ARGS__>(args)...);                  \
            for (auto &p : __##name##_aop_after_func_vec__) {                 \
                std::get<0>(p)();                                             \
            }                                                                 \
        } else {                                                              \
            decltype(auto) ret =                                              \
                name(std::forward<__SF_OBJECT_ARGS__>(args)...);              \
            for (auto &p : __##name##_aop_after_func_vec__) {                 \
                std::get<0>(p)();                                             \
            }                                                                 \
            return ret;                                                       \
        }                                                                     \
    }

/*
 * sf_aop_before_bind AOP调用前注册
 */
#define sf_aop_before_bind(objptr, name, func) \
    (objptr)->__sf_aop_before_add_helper(      \
        (objptr)->__mu_##name##_aop__,         \
        (objptr)->__##name##_aop_before_func_vec__, func)

/*
 * sf_aop_after_bind AOP调用后注册
 */
#define sf_aop_after_bind(objptr, name, func) \
    (objptr)->__sf_aop_after_add_helper(      \
        (objptr)->__mu_##name##_aop__,        \
        (objptr)->__##name##_aop_after_func_vec__, func)

/*
 * sf_aop_before_unbind AOP调用前反注册
 */
#define sf_aop_before_unbind(objptr, name, bind_id) \
    (objptr)->__sf_aop_unbind_helper(               \
        (objptr)->__mu_##name##_aop__,              \
        (objptr)->__##name##_aop_before_func_vec__, bind_id);

/*
 * sf_aop_after_unbind AOP调用后反注册
 */
#define sf_aop_after_unbind(objptr, name, bind_id) \
    (objptr)->__sf_aop_unbind_helper(              \
        (objptr)->__mu_##name##_aop__,             \
        (objptr)->__##name##_aop_after_func_vec__, bind_id);

/*
 * sf_bind_signal 信号绑定
 */
#define sf_bind_signal(objptr, name, func, single_thread)                    \
    (objptr)->__sf_bind_helper((objptr)->__mu_##name##_signal_,              \
                               (objptr)->__##name##_signal_func_vec__, func, \
                               single_thread)

/*
 * sf_unbind_signal 信号解绑
 */
#define sf_unbind_signal(objptr, name, bind_id) \
    (objptr)->__sf_signal_unbind_helper(        \
        (objptr)->__mu_##name##_signal_,        \
        (objptr)->__##name##_signal_func_vec__, bind_id);

namespace skyfire {
/**
 *  @brief 元对象
 */
class sf_object {
   public:
    template <typename _VectorType, typename _FuncType>
    int __sf_bind_helper(std::recursive_mutex &mu, _VectorType &vec,
                         _FuncType func, bool single_thread);

    template <typename _VectorType>
    void __sf_signal_unbind_helper(std::recursive_mutex &mu, _VectorType &vec,
                                   int bind_id);

    template <typename _VectorType, typename _FuncType>
    int __sf_aop_before_add_helper(std::recursive_mutex &mu, _VectorType &vec,
                                   _FuncType func);

    template <typename _VectorType, typename _FuncType>
    int __sf_aop_after_add_helper(std::recursive_mutex &mu, _VectorType &vec,
                                  _FuncType func);

    template <typename _VectorType>
    void __sf_aop_unbind_helper(std::recursive_mutex &mu, _VectorType &vec,
                                int bind_id);

    virtual ~sf_object();

   protected:
    sf_msg_queue *__p_msg_queue__ = sf_msg_queue::instance();
};

}    // namespace skyfire

#pragma clang diagnostic pop