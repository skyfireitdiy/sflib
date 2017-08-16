#include "sf_single_instance.h"
#include <iostream>

class TestClass:public skyfire::sf_single_instance<TestClass>
{
private:
	TestClass()
	{
	}

public:
	void func() const	{
		std::cout << "hello world" << std::endl;
	} 
	friend class skyfire::sf_single_instance<TestClass>;
};


int main()
{
	auto p = TestClass::get_instance();
	p->func();
}