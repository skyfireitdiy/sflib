/*
 * sf_timer 定时器
 */

#pragma once

#include "sf_nocopy.h"
#include "sf_object.h"
#include <atomic>

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
            std::thread([=]()
                        {
                            while (true)
                            {
                                std::this_thread::sleep_for(std::chrono::milliseconds(ms));
                                if (running__)
                                {
                                    timeout();
                                    if (once)
                                    {
                                        break;
                                    }
                                }
                                else
                                {
                                    break;
                                }
                            }
                            running__ = false;
                        }).detach();
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
    };
}
