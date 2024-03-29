
#pragma once
#include "sflib/tools/utils.h"

#include <atomic>
#include <deque>
#include <functional>
#include <future>

namespace skyfire
{
class thread_pool : public make_instance_t<thread_pool>
{
public:
    explicit thread_pool(size_t thread_count = std::thread::hardware_concurrency());
    ~thread_pool();
    template <typename Func, typename... Args>
    std::future<std::invoke_result_t<std::decay_t<Func>, std::decay_t<Args>...>> add_task(Func func, Args&&... args);
    void                                                                         pause();
    void                                                                         resume();
    void                                                                         add_thread(size_t thread_num = 1);
    size_t                                                                       thread_count() const;
    size_t                                                                       busy_thread_count() const;
    void                                                                         clear_thread();
    void                                                                         clear_task();
    void                                                                         wait_all_task_finished();

private:
    std::atomic<size_t>                       thread_count__ { 0 };
    std::deque<std::function<void()>>         task_deque__;
    static void                               thread_run__(thread_pool* this__);
    std::mutex                                mu_task_deque__;
    std::vector<std::shared_ptr<std::thread>> thread_vec__;
    bool                                      is_pause__ { false };
    bool                                      is_exit__ { false };
    std::atomic_int                           busy_thread_num__ { 0 };
    std::mutex                                mu_thread__cv__;
    std::condition_variable                   thread_cv__;
    std::mutex                                mu_wait_finish__;
    std::condition_variable                   wait_finish_cv__;
    void                                      add_thread__(size_t num);
};

template <typename Func, typename... Args>
std::future<std::invoke_result_t<std::decay_t<Func>, std::decay_t<Args>...>> thread_pool::add_task(Func func, Args&&... args)
{
    using _Ret = std::invoke_result_t<Func, Args...>;
    auto task  = std::make_shared<std::packaged_task<_Ret()>>(
        std::bind(func, std::forward<Args>(args)...));
    auto fu = task->get_future();
    {
        std::lock_guard lck(mu_task_deque__);
        task_deque__.push_back([=]() { (*task)(); });
    }
    thread_cv__.notify_all();
    return fu;
}
} // namespace skyfire
