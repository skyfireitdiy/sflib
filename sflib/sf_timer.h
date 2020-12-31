
/**
* @version 1.0.0
* @author skyfire
* @file sf_timer.h
*/

/*
 * timer 定时器
 */
#pragma once

#include "sf_nocopy.h"
#include "sf_object.hpp"
#include <atomic>

namespace skyfire
{
/**
 *  @brief 定时器
 */
class timer : public nocopy<object>
{
public:
    /**
     * timeout 定时器触发信号
     */
    sf_singal(timeout);

public:
    /**
     * @brief start 启动定时器
     * @param ms 毫秒
     * @param once 是否是一次性定时器
     */
    void start(int ms, bool once = false);

    /**
     * @brief stop 停止定时器
     */
    void stop();

    /**
     * @brief is_active 是否是活动的定时器
     * @return 是否活动
     */
    bool is_active() const;

private:
    std::atomic<bool> running__ { false };
    std::thread::id   current_timer_thread__;
};

} // namespace skyfire
