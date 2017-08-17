#include "sf_watch_time.h"
#include <string>
#include <windows.h>

skyfire::sf_watch_time<std::string> wc;

void func1()
{
	skyfire::check_point<> lck1(__FUNCTION__ " " + std::to_string(__LINE__), wc);
	int sum = 0;
	for (int i = 0; i < 1000; ++i)
	{
		skyfire::check_point<> lck1(__FUNCTION__ " "+ std::to_string(__LINE__), wc);
		sum += i;
	}
	for (int i = 0; i < 10000; ++i)
	{
		skyfire::check_point<> lck1(__FUNCTION__ " " + std::to_string(__LINE__), wc);
		sum += i;
	}
}


void func2()
{
	skyfire::check_point<> lck1(__FUNCTION__ " " + std::to_string(__LINE__), wc);
	Sleep(1000);
}

int main()
{
	func1();
	func2();
}