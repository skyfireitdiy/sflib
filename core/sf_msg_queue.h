
/**
* @version 1.0.0
* @author skyfire
* @mail skyfireitdiy@hotmail.com
* @see http://github.com/skyfireitdiy/sflib
* @file sf_msg_queue.h

* sflib第一版本发布
* 版本号1.0.0
* 发布日期：2018-10-22
*/

/*
 * sf_msg_queue 消息队列
 */

#pragma once

#include "sf_stdc++.h"
#include "tools/sf_single_instance.hpp"

namespace skyfire {
/**
 *  @brief 消息队列
 */
class sf_msg_queue final {
   public:
    SF_SINGLE_TON(sf_msg_queue);

   private:
    sf_msg_queue() = default;
    std::list<std::pair<void *, std::function<void()>>> func_data__;
    std::mutex mu_func_data_op__;

    std::condition_variable wait_cond__;

   public:
    /**
     * 增加消息
     * @param id 对象
     * @param func 要调用的函数
     */
    void add_msg(void *id, std::function<void()> func);

    /**
     * 删除对象的所有消息
     * @param id 对象
     */
    void remove_msg(void *id);

    /**
     * 清空消息
     */
    void clear();

    /**
     * 获取一条消息
     * @return
     */
    std::function<void()> take();

    /**
     * 判断是否队列为空
     * @return 是否为空
     */
    bool empty() const;

    /**
     * 等待消息到来
     */
    void wait_msg();

    /**
     * 添加一个空消息
     */
    void add_empty_msg();
};

}    // namespace skyfire
