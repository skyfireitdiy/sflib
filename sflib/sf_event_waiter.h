
/**
* @version 1.0.0
* @author skyfire
* @file event_waiter.h
*/

/*
 * event_waiter 事件等待
 */

#pragma once
#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCUnusedGlobalDeclarationInspection"

#include "sf_meta.hpp"
#include "sf_nocopy.h"
#include "sf_object.hpp"
#include <condition_variable>
#include <mutex>

/*
 * @brief wait 等待事件
 * @param obj 对象
 * @param name 信号
 */
#define sf_wait(obj, name)                                                            \
    {                                                                                 \
        auto p_waiter = skyfire::make_waiter((obj)->__##name##_signal_func_vec__); \
        auto bind_id = sf_bind(obj, name, p_waiter->make_quit_func__(), true);        \
        p_waiter->wait__();                                                           \
        sf_unbind(obj, name, bind_id);                                                \
    }

namespace skyfire {
/**
 * 等待对象
 * @tparam ARGS 等待参数列表
 */
template <typename... ARGS>
class __event_waiter__ : public nocopy<> {
private:
    std::mutex mu_cond__;
    std::condition_variable cond__;

public:
    /**
     * 生成退出函数
     * @return 退出函数
     */
    auto make_quit_func__();

    /**
     * @brief wait__ 等待
     */
    void wait__();

    /**
     * @brief quit__ 退出等待
     */
    void quit__();
};

/**
 * 生成等待器
 * @tparam ARGS 参数表
 * @return 等待器
 */
template <typename... ARGS>
std::shared_ptr<__event_waiter__<ARGS...>> make_waiter(
    const std::vector<std::tuple<std::function<void(ARGS...)>, bool, int>>&);

} // namespace skyfire

#pragma clang diagnostic pop