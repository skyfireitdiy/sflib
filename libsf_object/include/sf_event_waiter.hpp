
/**
* @version 1.0.0
* @author skyfire
* @email skyfireitdiy@hotmail.com
* @see http://github.com/skyfireitdiy/sflib
* @file sf_event_waiter.hpp

* sflib第一版本发布
* 版本号1.0.0
* 发布日期：2018-10-22
*/

/*
 * sf_event_waiter 事件等待
 */

#pragma once

#include "sf_event_waiter.h"



/*
 * @brief sf_wait 等待事件
 * @param obj 对象
 * @param name 信号
 */
#define sf_wait(obj, name)                                                        \
{                                                                                \
    auto p_waiter = sf_make_waiter((obj)->__##name##_func_vec__);                \
    auto bind_id = sf_bind_signal(obj,name,p_waiter->__make_quit_func(), true);  \
    p_waiter->wait();                                                            \
    sf_unbind_signal(obj,name,bind_id);                                          \
}                                                                                \


namespace skyfire {

    template<typename... ARGS>
    void sf_event_waiter<ARGS...>::wait() {
        std::unique_lock<std::mutex> lck(mu_cond__);
        cond__.wait(lck);
    }

    template<typename... ARGS>
    template<size_t... Index>
    auto sf_event_waiter<ARGS...>::__make_quit_func_helper(std::index_sequence<Index...>) {
        // WARNING _Placeholder不是标准类型
        return [=]() {
            quit(make_placeholders<Index + 1>()...);
        };
    }

    template<typename... ARGS>
    auto sf_event_waiter<ARGS...>::__make_quit_func() {
        return __make_quit_func_helper(std::make_index_sequence<sizeof...(ARGS)>{});
    }

    template<typename... ARGS>
    void sf_event_waiter<ARGS...>::quit(ARGS...) {
        cond__.notify_one();
    }

    template<typename... ARGS>
    sf_event_waiter<ARGS...>::sf_event_waiter() {
    }

    template<typename... ARGS>
    std::shared_ptr<sf_event_waiter<ARGS...>>
    sf_make_waiter(const std::vector<std::tuple<std::function<void(ARGS...)>, bool, int>> &) {
        return std::make_shared<sf_event_waiter<ARGS...>>();
    }
}
