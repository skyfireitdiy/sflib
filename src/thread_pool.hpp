
#pragma once
#include "thread_pool.h"
#include "utils.h"
namespace skyfire
{
inline thread_pool::thread_pool(size_t thread_count)
{
    if (thread_count < 1)
    {
        thread_count = 1;
    }
    add_thread__(thread_count);
}
inline void thread_pool::thread_run__(thread_pool* this__)
{
    while (true)
    {
        while ((!this__->is_exit__) && (this__->is_pause__ || this__->task_deque__.empty()))
        {
            ++this__->busy_thread_num__;
            std::unique_lock<std::mutex> lck_cv(this__->mu_thread__cv__);
            this__->thread_cv__.wait(lck_cv);
            --this__->busy_thread_num__;
        }
        if (this__->is_exit__)
        {
            break;
        }
        {
            auto                  flag_empty = true;
            std::function<void()> task;
            {
                std::lock_guard<std::mutex> lck(this__->mu_task_deque__);
                if (!this__->task_deque__.empty())
                {
                    flag_empty = false;
                    task       = this__->task_deque__.front();
                    this__->task_deque__.pop_front();
                }
            }
            if (!flag_empty)
            {
                task();
                this__->wait_finish_cv__.notify_all();
            }
        }
    }
}
inline void thread_pool::add_thread__(size_t num)
{
    if (num < 1)
    {
        num = 1;
    }
    thread_count__ += num;
    for (decltype(num) i = 0; i < num; ++i)
    {
        auto p_thread = std::make_shared<std::thread>(thread_run__, this);
        thread_vec__.push_back(p_thread);
    }
}
inline void thread_pool::wait_all_task_finished()
{
    while (!task_deque__.empty())
    {
        std::unique_lock<std::mutex> lck_cv(mu_wait_finish__);
        wait_finish_cv__.wait(lck_cv);
    }
}
inline void thread_pool::clear_task()
{
    std::lock_guard<std::mutex> lck(mu_task_deque__);
    task_deque__.clear();
}
inline void thread_pool::clear_thread()
{
    is_pause__ = false;
    is_exit__  = true;
    thread_cv__.notify_all();
    for (auto& p : thread_vec__)
    {
        p->join();
    }
    thread_vec__.clear();
}
inline size_t thread_pool::busy_thread_count() const
{
    return static_cast<size_t>(busy_thread_num__);
}
inline size_t thread_pool::thread_count() const { return thread_count__; }
inline void   thread_pool::add_thread(const size_t thread_num)
{
    add_thread__(thread_num);
}
inline void thread_pool::resume()
{
    is_pause__ = false;
    thread_cv__.notify_all();
}
template <typename Func, typename... Args>
std::future<std::invoke_result_t<std::decay_t<Func>, std::decay_t<Args>...>> thread_pool::add_task(Func func, Args&&... args)
{
    using _Ret = std::invoke_result_t<Func, Args...>;
    auto task  = std::make_shared<std::packaged_task<_Ret()>>(
        std::bind(func, std::forward<Args>(args)...));
    auto fu = task->get_future();
    {
        std::lock_guard<std::mutex> lck(mu_task_deque__);
        task_deque__.push_back([=]() { (*task)(); });
    }
    thread_cv__.notify_all();
    return fu;
}
inline thread_pool::~thread_pool() { clear_thread(); }
inline void thread_pool::pause() { is_pause__ = true; }
} // namespace skyfire
