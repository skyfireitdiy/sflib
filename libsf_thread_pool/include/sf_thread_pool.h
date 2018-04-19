#pragma once
#include <functional>
#include <atomic>
#include <deque>
#include <mutex>
#include <vector>
#include <memory>
#include <thread>
#include <condition_variable>

namespace skyfire
{

	class sf_thread_pool
	{
	public:

		explicit sf_thread_pool(size_t thread_count = 4)
		{
			add_thread__(thread_count);
		}

		~sf_thread_pool()
		{
			clear_thread();
		}

		template <typename Func,typename ... T>
		void add_task(Func func, T&&...args)
		{
			std::lock_guard<std::mutex> lck(mu_task_deque__);
			task_deque__.push_back(std::bind(func, std::forward<T>(args)...));
			thread_cv__.notify_all();
		}

        void add_task(std::function<void()> func)
        {
            std::lock_guard<std::mutex> lck(mu_task_deque__);
            task_deque__.push_back(func);
            thread_cv__.notify_all();
        }

		void pause()
		{
			is_pause__ = true;
		}

		void resume()
		{
			is_pause__ = false;
			thread_cv__.notify_all();
		}

		void add_thread(size_t thread_num = 1)
		{
			add_thread__(thread_num);
		}

		size_t get_thread_count() const
		{
			return thread_count__;
		}

		size_t get_busy_thread_count()const
		{
			return busy_thread_num__;
		}

		void clear_thread()
		{
			is_pause__ = false;
			is_exit__ = true;
			thread_cv__.notify_all();
			for (auto &p : thread_vec__)
			{
				p->join();
			}
			thread_vec__.clear();
		}

		void clear_task()
		{
			std::lock_guard<std::mutex> lck(mu_task_deque__);
			task_deque__.clear();
		}

		void wait_all_task_finished()
		{
			while (!task_deque__.empty())
			{
				std::unique_lock<std::mutex> lck_cv(mu_wait_finish__);
				wait_finish_cv__.wait(lck_cv);
			}
		}

	private:
		std::atomic<size_t > thread_count__{ 4 };
		std::deque<std::function<void()>> task_deque__;

		static void thread_run__(sf_thread_pool *this__)
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
					auto flag_empty = true;
					std::function<void()> task;
					{
						std::lock_guard<std::mutex> lck(this__->mu_task_deque__);
						if (!this__->task_deque__.empty())
						{
							flag_empty = false;
							task = this__->task_deque__.front();
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

		std::mutex mu_task_deque__;
		std::vector<std::shared_ptr<std::thread>> thread_vec__;
		bool is_pause__{ false };
		bool is_exit__{ false };
		std::atomic_int busy_thread_num__{ 0 };
		std::mutex mu_thread__cv__;
		std::condition_variable thread_cv__;
		std::mutex mu_wait_finish__;
		std::condition_variable wait_finish_cv__;

		void add_thread__(size_t num)
		{
			if (num < 1)
			{
				num = 1;
			}
			thread_count__ += num;
			for (auto i = 0; i < num; ++i)
			{
				auto p_thread = std::make_shared<std::thread>(thread_run__, this);
				thread_vec__.push_back(p_thread);
			}
		}
	};





}