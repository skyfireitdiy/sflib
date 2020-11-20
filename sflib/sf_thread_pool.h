
/**
* @version 1.0.0
* @author skyfire
* @file sf_thread_pool.h
*/

/*
 * thread_pool 线程池
 */
#pragma once
#pragma clang diagnostic push
#pragma ide diagnostic   ignored "OCUnusedGlobalDeclarationInspection"

#include "sf_stdc++.h"
#include "sf_utils.h"

namespace skyfire
{

/**
 *  @brief  线程池
 */
class thread_pool : public make_instance_t<thread_pool>
{
public:
    /**
     * @brief thread_pool 构造函数
     * @param thread_count 线程数量
     */
    explicit thread_pool(size_t thread_count = std::thread::hardware_concurrency());

    ~thread_pool();

    /**
     * @brief add_task 添加任务
     * @param func 函数
     * @param args 参数
     */
    template <typename Func, typename... Args>
    auto add_task(Func func, Args&&... args);

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
    std::atomic<size_t>               thread_count__ { 0 };
    std::deque<std::function<void()>> task_deque__;

    static void thread_run__(thread_pool* this__);

    std::mutex                                mu_task_deque__;
    std::vector<std::shared_ptr<std::thread>> thread_vec__;
    bool                                      is_pause__ { false };
    bool                                      is_exit__ { false };
    std::atomic_int                           busy_thread_num__ { 0 };
    std::mutex                                mu_thread__cv__;
    std::condition_variable                   thread_cv__;
    std::mutex                                mu_wait_finish__;
    std::condition_variable                   wait_finish_cv__;

    void add_thread__(size_t num);
};

} // namespace skyfire

#pragma clang diagnostic pop