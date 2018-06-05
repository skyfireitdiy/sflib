/*
 * sf_event_waiter 事件等待
 */

#pragma once

#include "sf_nocopy.h"
#include "sf_object.h"
#include <condition_variable>
#include <mutex>


/*
 * @brief sf_wait 等待事件
 * @param obj 对象
 * @param name 信号
 */
#define sf_wait(obj,name)                                                        \
{                                                                                \
    auto p_waiter = sf_make_waiter((obj)->__##name##_func_vec__);                \
    auto bind_id = sf_bind_signal(obj,name,p_waiter->__make_quit_func(), true);  \
    p_waiter->wait();                                                            \
    sf_unbind_signal(obj,name,bind_id);                                          \
}                                                                                \


namespace skyfire
{
    template <typename ... ARGS>
    class sf_event_waiter : public sf_nocopy<>
    {
    private:
        std::mutex mu_cond__;
        std::condition_variable cond__;
    public:
        /**
         * @brief sf_event_waiter 构造一个事件等待对象
         */
        sf_event_waiter()
        {
        }

        /**
         * @brief wait 等待
         */
        void wait()
        {
            std::unique_lock<std::mutex> lck(mu_cond__);
            cond__.wait(lck);
        }

        template<std::size_t... Index>
        auto __make_quit_func_helper(std::index_sequence<Index...>)
        {
            // WARNING _Placeholder不是标准类型
            return std::bind(&sf_event_waiter<ARGS...>::quit, this, std::_Placeholder<Index + 1>()...);
        }

        auto __make_quit_func()
        {
            return __make_quit_func_helper(std::make_index_sequence<sizeof...(ARGS)>{});
            //using namespace std::placeholders;
            //if constexpr(sizeof...(ARGS) == 0)return std::bind(&sf_event_waiter<ARGS...>::quit, this);
            //else if constexpr (sizeof...(ARGS) == 1 )return std::bind(&sf_event_waiter<ARGS...>::quit, this, _1);
            //else if constexpr (sizeof...(ARGS) == 2 )return std::bind(&sf_event_waiter<ARGS...>::quit, this, _1,_2);
            //else if constexpr (sizeof...(ARGS) == 3 )return std::bind(&sf_event_waiter<ARGS...>::quit, this, _1,_2,_3);
            //else if constexpr (sizeof...(ARGS) == 4 )return std::bind(&sf_event_waiter<ARGS...>::quit, this, _1,_2,_3,_4);
            //else if constexpr (sizeof...(ARGS) == 5 )return std::bind(&sf_event_waiter<ARGS...>::quit, this, _1,_2,_3,_4,_5);
            //else if constexpr (sizeof...(ARGS) == 6 )return std::bind(&sf_event_waiter<ARGS...>::quit, this, _1,_2,_3,_4,_5,_6);
            //else if constexpr (sizeof...(ARGS) == 7 )return std::bind(&sf_event_waiter<ARGS...>::quit, this, _1,_2,_3,_4,_5,_6,_7);
            //else if constexpr (sizeof...(ARGS) == 8 )return std::bind(&sf_event_waiter<ARGS...>::quit, this, _1,_2,_3,_4,_5,_6,_7,_8);
            //else if constexpr (sizeof...(ARGS) == 9 )return std::bind(&sf_event_waiter<ARGS...>::quit, this, _1,_2,_3,_4,_5,_6,_7,_8,_9);
            //else if constexpr (sizeof...(ARGS) == 10)return std::bind(&sf_event_waiter<ARGS...>::quit, this, _1,_2,_3,_4,_5,_6,_7,_8,_9,_10);
            //else if constexpr (sizeof...(ARGS) == 11)return std::bind(&sf_event_waiter<ARGS...>::quit, this, _1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11);
            //else if constexpr (sizeof...(ARGS) == 12)return std::bind(&sf_event_waiter<ARGS...>::quit, this, _1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,_12);
            //else if constexpr (sizeof...(ARGS) == 13)return std::bind(&sf_event_waiter<ARGS...>::quit, this, _1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,_12,_13);
            //else if constexpr (sizeof...(ARGS) == 14)return std::bind(&sf_event_waiter<ARGS...>::quit, this, _1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,_12,_13,_14);
            //else if constexpr (sizeof...(ARGS) == 15)return std::bind(&sf_event_waiter<ARGS...>::quit, this, _1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,_12,_13,_14,_15);
            //else if constexpr (sizeof...(ARGS) == 16)return std::bind(&sf_event_waiter<ARGS...>::quit, this, _1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,_12,_13,_14,_15,_16);
            //else if constexpr (sizeof...(ARGS) == 17)return std::bind(&sf_event_waiter<ARGS...>::quit, this, _1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,_12,_13,_14,_15,_16,_17);
            //else if constexpr (sizeof...(ARGS) == 18)return std::bind(&sf_event_waiter<ARGS...>::quit, this, _1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,_12,_13,_14,_15,_16,_17,_18);
            //else if constexpr (sizeof...(ARGS) == 19)return std::bind(&sf_event_waiter<ARGS...>::quit, this, _1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,_12,_13,_14,_15,_16,_17,_18,_19);
            //else if constexpr (sizeof...(ARGS) == 20)return std::bind(&sf_event_waiter<ARGS...>::quit, this, _1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,_12,_13,_14,_15,_16,_17,_18,_19,_20);
            //else if constexpr (sizeof...(ARGS) == 21)return std::bind(&sf_event_waiter<ARGS...>::quit, this, _1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,_12,_13,_14,_15,_16,_17,_18,_19,_20,_21);
            //else if constexpr (sizeof...(ARGS) == 22)return std::bind(&sf_event_waiter<ARGS...>::quit, this, _1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,_12,_13,_14,_15,_16,_17,_18,_19,_20,_21,_22);
            //else if constexpr (sizeof...(ARGS) == 23)return std::bind(&sf_event_waiter<ARGS...>::quit, this, _1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,_12,_13,_14,_15,_16,_17,_18,_19,_20,_21,_22,_23);
            //else if constexpr (sizeof...(ARGS) == 24)return std::bind(&sf_event_waiter<ARGS...>::quit, this, _1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,_12,_13,_14,_15,_16,_17,_18,_19,_20,_21,_22,_23,_24);
            //else if constexpr (sizeof...(ARGS) == 25)return std::bind(&sf_event_waiter<ARGS...>::quit, this, _1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,_12,_13,_14,_15,_16,_17,_18,_19,_20,_21,_22,_23,_24,_25);
        }

        /**
         * @brief quit 退出等待
         */
        void quit(ARGS ...)
        {
            cond__.notify_one();
        }
    };


    template<typename...ARGS>
    std::shared_ptr<sf_event_waiter<ARGS...>> sf_make_waiter(const std::vector<std::tuple<std::function<void(ARGS...)>, bool, int>>&)
    {
        return std::make_shared<sf_event_waiter<ARGS...>>();
    }
}
