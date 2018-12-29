#include "sf_object_factory.hpp"
#include "sf_stdc++.h"
using namespace skyfire;

class A{
private:
    int num;
public:
    A(int n) : num(n)
    {
	    std::cout<<"A()"<< std::endl;
    }

    void print() const
    {
	    std::cout<<"n is "<< num<< std::endl;
    }
};

class B{
public:
    B()
    {
	    std::cout<<"B()"<< std::endl;
    }
    void print() const
    {
	    std::cout<<"B object"<< std::endl;
    }
};



int main()
{
    // 1. 获取工厂对象
    sf_object_factory factory;

    // 2. 设置创建前回调
    factory.set_before_create_callback([](const std::string& type){
	    std::cout<<"before " + type + " create"<< std::endl;
    });

    // 3. 设置创建后回调
    factory.set_after_create_callback([](const std::string& type){
	    std::cout<<"after " + type + " create"<< std::endl;
    });

    // 4. 创建对象

    auto a_obj = factory.get_object<A>("A", 20);
    auto b_obj = factory.get_object<B>("B");

    // 5. 调用函数
    a_obj->print();
    b_obj->print();

}