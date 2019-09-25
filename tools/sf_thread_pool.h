
/**
* @version 1.0.0
* @author skyfire
* @mail skyfireitdiy@hotmail.com
* @see http://github.com/skyfireitdiy/sflib
* @file sf_thread_pool.h

* sflib第一版本发布
* 版本号1.0.0
* 发布日期：2018-10-22
*/

/*
 * sf_thread_pool 线程池
 */
#pragma once
#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCUnusedGlobalDeclarationInspection"

#include "core/sf_stdc++.h"
#include "sf_utils.h"

namespace skyfire {

/**
 *  @brief  线程池
 */
class sf_thread_pool : public sf_make_instance_t<sf_thread_pool>{
   public:
    /**
     * @brief sf_thread_pool 构造函数
     * @param thread_count 线程数量
     */
    explicit sf_thread_pool(size_t thread_count = 4);

    ~sf_thread_pool();

    /**
     * @brief add_task 添加任务
     * @param func 函数
     * @param args 参数
     */
    template <typename Func, typename... Args>
    auto add_task(Func func, Args &&... args);

    /**
     * @brief pause 暂停
     */
    void pause();

    /**
     * @brief resume 继续
     */
    void resume();

    /**
     * @brief add_thread 添加调度线程
     * @param thread_num 添加调度线程数量
     */
    void add_thread(size_t thread_num = 1);

    /**
     * @brief thread_count 获取调度线程数量
     * @return 调度线程数量
     */
    size_t thread_count() const;

    /**
     * @brief busy_thread_count 获取繁忙线程数量
     * @return
     */
    size_t busy_thread_count() const;

    /**
     * @brief clear_thread 清除所有的调度线程
     */
    void clear_thread();

    /**
     * @brief clear_task 清空所有任务
     */
    void clear_task();

    /**
     * @brief wait_all_task_finished 等待所有任务结束
     */
    void wait_all_task_finished();

   private:
    std::atomic<size_t> thread_count__{4};
    std::deque<std::function<void()>> task_deque__;

    static void thread_run__(sf_thread_pool *this__);

    std::mutex mu_task_deque__;
    std::vector<std::shared_ptr<std::thread>> thread_vec__;
    bool is_pause__{false};
    bool is_exit__{false};
    std::atomic_int busy_thread_num__{0};
    std::mutex mu_thread__cv__;
    std::condition_variable thread_cv__;
    std::mutex mu_wait_finish__;
    std::condition_variable wait_finish_cv__;

    void add_thread__(size_t num);
};

}    // namespace skyfire

#pragma clang diagnostic pop