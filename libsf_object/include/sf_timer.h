/*
 * sf_timer 定时器
 */

#pragma once

#include "sf_nocopy.h"
#include "sf_object.h"
#include <atomic>
#include <random>

namespace skyfire
{
    class sf_timer : public sf_nocopy<sf_object>
    {
    public:
        /**
         * timeout 定时器触发信号
         */
        SF_REG_SIGNAL(timeout);
    public:

        /**
         * @brief sf_timer 构造函数
         */
        sf_timer(): e__(rd__()), ed__(0,INT_MAX){

        }

        /**
         * @brief start 启动定时器
         * @param ms 毫秒
         * @param once 是否是一次性定时器
         */
        void start(int ms, bool once = false)
        {
            if(running__)
            {
                return;
            }
            running__ = true;
            curr_work_timer__ = ed__(e__);
            std::thread([=](int current_timer, bool is_once)
                        {
                            while (true)
                            {
                                std::this_thread::sleep_for(std::chrono::milliseconds(ms));
                                if(current_timer != curr_work_timer__)
                                {
                                    return;
                                }
                                if (running__)
                                {
                                    timeout();
                                    if (is_once)
                                    {
                                        running__ = false;
                                        return;
                                    }
                                }
                                else
                                {
                                    break;
                                }
                            }

                        }, curr_work_timer__, once).detach();
        }

        /**
         * @brief stop 停止定时器
         */
        void stop()
        {
            running__ = false;
        }

        /**
         * @brief is_active 是否是活动的定时器
         * @return 是否活动
         */
        bool is_active()
        {
            return running__;
        }

    private:
        std::atomic<bool> running__ {false};
        int curr_work_timer__;
        std::random_device rd__;
        std::default_random_engine e__;
        std::uniform_int_distribution<int> ed__;
    };
}
