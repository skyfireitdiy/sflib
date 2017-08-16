#include "sf_thread_pool.h"
#include <iostream>
#include <windows.h>
#include "../libsf_empty_class/sfempty_class.h"

std::mutex g_mu;

int add_sum = 0;
int mul_sum = 0;

void add(int a, int b)
{
	add_sum += (a + b);
}

void mul(int a, int b)
{
	mul_sum += a*b;
}

int main()
{
	skyfire::sf_thread_pool<int, int> pool(1);
	std::cout << GetTickCount() << std::endl;
	for (auto i = 0; i < 100000; ++i)
	{
		pool.add_task(add, i, i + 1);
		pool.add_task(mul, i, i + 1);
	}
	pool.wait_all_task_finished();
	std::cout << add_sum << std::ends << mul_sum << std::endl;
	std::cout << GetTickCount() << std::endl;

	add_sum = mul_sum = 0;
	std::cout << GetTickCount() << std::endl;
	for (auto i = 0; i < 100000; ++i)
	{
		add(i, i + 1);
		mul(i, i + 1);
	}
	std::cout << add_sum << std::ends << mul_sum << std::endl;
	std::cout << GetTickCount() << std::endl;

}