/*
 * sf_event_waiter 事件等待
 */

#pragma once

#include "sf_nocopy.h"
#include "sf_object.hpp"
#include "sf_meta.h"
#include <condition_variable>
#include <mutex>



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
    template<typename ... ARGS>
    class sf_event_waiter : public sf_nocopy<> {
    private:
        std::mutex mu_cond__;
        std::condition_variable cond__;
    public:
        /**
         * @brief sf_event_waiter 构造一个事件等待对象
         */
        sf_event_waiter() {
        }

        /**
         * @brief wait 等待
         */
        void wait() {
            std::unique_lock<std::mutex> lck(mu_cond__);
            cond__.wait(lck);
        }

        template<std::size_t... Index>
        auto __make_quit_func_helper(std::index_sequence<Index...>) {
            // WARNING _Placeholder不是标准类型
            return [=]() {
                quit(make_placeholders<Index + 1>()...);
            };
        }

        auto __make_quit_func() {
            return __make_quit_func_helper(std::make_index_sequence<sizeof...(ARGS)>{});
        }

        /**
         * @brief quit 退出等待
         */
        void quit(ARGS ...) {
            cond__.notify_one();
        }
    };


    template<typename...ARGS>
    std::shared_ptr<sf_event_waiter<ARGS...>>
    sf_make_waiter(const std::vector<std::tuple<std::function<void(ARGS...)>, bool, int>> &) {
        return std::make_shared<sf_event_waiter<ARGS...>>();
    }
}
