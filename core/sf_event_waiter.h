
/**
* @version 1.0.0
* @author skyfire
* @mail skyfireitdiy@hotmail.com
* @see http://github.com/skyfireitdiy/sflib
* @file sf_event_waiter.h

* sflib第一版本发布
* 版本号1.0.0
* 发布日期：2018-10-22
*/

/*
 * sf_event_waiter 事件等待
 */

#pragma once
#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCUnusedGlobalDeclarationInspection"

#include <condition_variable>
#include <mutex>
#include "sf_object.hpp"
#include "tools/sf_meta.hpp"
#include "tools/sf_nocopy.h"

/*
 * @brief sf_wait 等待事件
 * @param obj 对象
 * @param name 信号
 */
#define SF_WAIT(obj, name)                                                 \
    {                                                                      \
        auto p_waiter =                                                    \
            skyfire::sf_make_waiter((obj)->__##name##_signal_func_vec__);  \
        auto bind_id =                                                     \
            sf_bind_signal(obj, name, p_waiter->__make_quit_func(), true); \
        p_waiter->wait();                                                  \
        sf_unbind_signal(obj, name, bind_id);                              \
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

    template <std::size_t... Index>
    auto __make_quit_func_helper(std::index_sequence<Index...>);

   public:
    /**
     * 生成退出函数
     * @return 退出函数
     */
    auto __make_quit_func();

    /**
     * @brief wait 等待
     */
    void wait();

    /**
     * @brief quit 退出等待
     */
    void quit(ARGS...);
};

/**
 * 生成等待器
 * @tparam ARGS 参数表
 * @return 等待器
 */
template <typename... ARGS>
std::shared_ptr<sf_event_waiter<ARGS...>> sf_make_waiter(
    const std::vector<std::tuple<std::function<void(ARGS...)>, bool, int>> &);

}    // namespace skyfire

#pragma clang diagnostic pop