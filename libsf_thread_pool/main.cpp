#include "sf_thread_pool.h"
#include <iostream>
#include <chrono>
#include "../libsf_empty_class/sfempty_class.h"

std::mutex g_mu;

std::atomic<int> add_sum {0};
std::atomic<int> mul_sum {0};

void add(int a, int b)
{
	add_sum += (a + b);
}

void mul(int a, int b)
{
	mul_sum += a*b;
}

const std::string getCurrentSystemTime()
{
	auto tt = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
	struct tm* ptm = localtime(&tt);
	char date[60] = {0};
	sprintf(date, "%d-%02d-%02d      %02d:%02d:%02d",
		(int)ptm->tm_year + 1900,(int)ptm->tm_mon + 1,(int)ptm->tm_mday,
		(int)ptm->tm_hour,(int)ptm->tm_min,(int)ptm->tm_sec);
	return std::string(date);
}

int main()
{
	skyfire::sf_thread_pool<int, int> pool(8);
	std::cout << getCurrentSystemTime()<<std::endl;
	for (auto i = 0; i < 100000; ++i)
	{
		pool.add_task(add, i, i + 1);
		pool.add_task(mul, i, i + 1);
	}
	pool.wait_all_task_finished();
	std::cout << add_sum << std::ends << mul_sum << std::endl;
	std::cout << getCurrentSystemTime()<<std::endl;

	add_sum = mul_sum = 0;
	std::cout << getCurrentSystemTime()<<std::endl;
	for (auto i = 0; i < 100000; ++i)
	{
		add(i, i + 1);
		mul(i, i + 1);
	}
	std::cout << add_sum << std::ends << mul_sum << std::endl;
	std::cout << getCurrentSystemTime()<<std::endl;

}