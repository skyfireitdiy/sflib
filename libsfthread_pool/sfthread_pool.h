#pragma once
#include <functional>
#include <atomic>
#include <deque>
#include <mutex>
#include <vector>

template<typename ... T>
class sfthread_pool
{
public:
	explicit sfthread_pool(size_t max_thread_count = 4);
	~sfthread_pool();
	void add_task(std::function<void(T ...)> func,T...args);
	void pause();
	void resume();
	void add_thread(size_t thread_num = 1);
	size_t get_thread_count() const;
	size_t get_busy_thread_count()const;
	void clear_thread();
	void clear_task();
	void wait_all_task_finished();

private:
	std::atomic<size_t > thread_count__{ 4 };
	std::deque<std::tuple<std::function<void(T ...)>, T...>> task_deque__;

	static void thread_run__(sfthread_pool *this__);

	std::mutex mu_task_deque__;
	std::vector<std::shared_ptr<std::thread>> thread_vec__;
	std::atomic_bool is_pause__{ false };
	std::atomic_bool is_exit__{ false };
	std::atomic_int busy_thread_num__{ 0 };
	std::mutex mu_thread__cv__;
	std::condition_variable thread_cv__;
	std::mutex mu_wait_finish__;
	std::condition_variable wait_finish_cv__;

	static void run_impl__(std::function<void(T ...)> func, T ...args);

	template<std::size_t... I>
	static void apply(std::tuple<std::function<void(T ...)>,T...> t, std::index_sequence<I...>);
	void add_thread__(int num);
};

template <typename ... T>
void sfthread_pool<T...>::add_thread__(int num)
{
	if(num<1)
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


template <typename ... T>
sfthread_pool<T...>::sfthread_pool(size_t thread_count):thread_count__(0)
{
	add_thread__(thread_count);
}

template <typename ... T>
sfthread_pool<T...>::~sfthread_pool()
{
	clear_thread();
}

template <typename ... T>
void sfthread_pool<T...>::add_task(std::function<void(T...)> func, T... args)
{
	std::lock_guard<std::mutex> lck(mu_task_deque__);
	task_deque__.push_back(std::make_tuple(func, args...));
	thread_cv__.notify_all();
}

template <typename ... T>
void sfthread_pool<T...>::pause()
{
	is_pause__ = true;
}

template <typename ... T>
void sfthread_pool<T...>::resume()
{
	is_pause__ = false;
	thread_cv__.notify_all();
}

template <typename ... T>
void sfthread_pool<T...>::add_thread(size_t thread_num)
{
	add_thread__(thread_num);
}

template <typename ... T>
size_t sfthread_pool<T...>::get_thread_count() const
{
	return thread_count__;
}

template <typename ... T>
size_t sfthread_pool<T...>::get_busy_thread_count() const
{
	return busy_thread_num__;
}

template <typename ... T>
void sfthread_pool<T...>::clear_thread()
{
	is_pause__ = false;
	is_exit__ = true;
	thread_cv__.notify_all();
	for(auto &p:thread_vec__)
	{
		p->join();
	}
	thread_vec__.clear();
}

template <typename ... T>
void sfthread_pool<T...>::clear_task()
{
	std::lock_guard<std::mutex> lck(mu_task_deque__);
	task_deque__.clear();
}

template <typename ... T>
void sfthread_pool<T...>::wait_all_task_finished()
{
	while (!task_deque__.empty())
	{
		std::unique_lock<std::mutex> lck_cv(mu_wait_finish__);
		wait_finish_cv__.wait(lck_cv);
	}
}


template <typename ... T>
void sfthread_pool<T...>::thread_run__(sfthread_pool* this__)
{
	while(true)
	{
		while ((!this__->is_exit__)&&(this__->is_pause__||this__->task_deque__.empty()))
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
			std::tuple<std::function<void(T ...)>, T...> task;
			std::lock_guard<std::mutex> lck(this__->mu_task_deque__);
			if (!this__->task_deque__.empty())
			{
				flag_empty = false;
				task = this__->task_deque__.front();
				this__->task_deque__.pop_front();
			}
			if (!flag_empty)
			{
				apply(task, std::make_index_sequence<std::tuple_size<std::tuple<std::function<void(T ...)>, T...>>::value>());
				this__->wait_finish_cv__.notify_all();
			}
		}
	}
}

template <typename ... T>
void sfthread_pool<T...>::run_impl__(std::function<void(T ...)> func, T ... args)
{
	func(args...);
}

template <typename ... T>
template <std::size_t... I>
void sfthread_pool<T...>::apply(std::tuple<std::function<void(T ...)>,T...> t, std::index_sequence<I...>)
{
	run_impl__(std::get<I>(t)...);
}


