
/**
* @version 1.0.0
* @author skyfire
* @file sf_event_waiter.h
*/

/*
 * sf_event_waiter 事件等待
 */

#pragma once
#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCUnusedGlobalDeclarationInspection"

#include "sf_object.hpp"
#include "sf_meta.hpp"
#include "sf_nocopy.h"
#include <condition_variable>
#include <mutex>

/*
 * @brief sf_wait 等待事件
 * @param obj 对象
 * @param name 信号
 */
#define SF_WAIT(obj, name)                                                            \
    {                                                                                 \
        auto p_waiter = skyfire::sf_make_waiter((obj)->__##name##_signal_func_vec__); \
        auto bind_id = sf_bind_signal(obj, name, p_waiter->make_quit_func__(), true); \
        p_waiter->wait();                                                             \
        sf_unbind_signal(obj, name, bind_id);                                         \
    }

namespace skyfire {
/**
 * 对待对象
 * @tparam ARGS 等待参数列表
 */
template <typename... ARGS>
class sf_event_waiter : public sf_nocopy<> {
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
     * @brief wait 等待
     */
    void wait();

    /**
     * @brief quit 退出等待
     */
    void quit();
};

/**
 * 生成等待器
 * @tparam ARGS 参数表
 * @return 等待器
 */
template <typename... ARGS>
std::shared_ptr<sf_event_waiter<ARGS...>> sf_make_waiter(
    const std::vector<std::tuple<std::function<void(ARGS...)>, bool, int>>&);

} // namespace skyfire

#pragma clang diagnostic pop