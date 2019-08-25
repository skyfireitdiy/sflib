
/**
* @version 1.0.0
* @author skyfire
* @mail skyfireitdiy@hotmail.com
* @see http://github.com/skyfireitdiy/sflib
* @file sf_timer.h

* sflib第一版本发布
* 版本号1.0.0
* 发布日期：2018-10-22
*/

/*
 * sf_timer 定时器
 */
#pragma once
#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCUnusedGlobalDeclarationInspection"

#include <atomic>
#include "core/sf_object.hpp"
#include "tools/sf_nocopy.h"

namespace skyfire {
/**
 *  @brief 定时器
 */
class sf_timer : public sf_nocopy<sf_object> {
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
    std::atomic<bool> running__{false};
    std::thread::id current_timer_thread__;
};

}    // namespace skyfire

#pragma clang diagnostic pop