#include "sf_watch_time.h"
#include <windows.h>

skyfire::sf_watch_time<int> wc;
using pt = skyfire::check_point<int>;

void func1()
{
	pt p(1, wc);
	auto sum = 0;
	for (auto i = 0; i < 1000; ++i)
	{
		pt p(2, wc);
		sum += i;
	}
	for (auto i = 0; i < 10000; ++i)
	{
		pt p(3, wc);
		sum += i;
	}
}


void func2()
{
	pt p(4, wc);
	Sleep(1000);
}

int main()
{
	func1();
	func2();
	wc.watch();
}