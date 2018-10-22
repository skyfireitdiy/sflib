
/**
* @version 1.0.0
* @author skyfire
* @email skyfireitdiy@hotmail.com
* @see http://github.com/skyfireitdiy/sflib
* @file test_meta_object.cpp

* sflib第一版本发布
* 版本号1.0.0
* 发布日期：2018-10-22
*/

#include "sf_meta_object.hpp"
#include <string>
#include <iostream>
using namespace skyfire;

struct A
{
    virtual void func() { std::cout<<"class A"<<std::endl; }
};
struct B: public A
{
    virtual void func() { std::cout<<"class B"<<std::endl; }
};
struct C: public A
{
    int data;
    C(int i) : data(i) {}
    virtual void func() { std::cout<<"class C, data:"<<data<<std::endl; }
};

int main()
{
    sf_meta_object o;
    o.reg_meta_type<A>("A");
    o.reg_meta_type<B>("B");
    o.reg_meta_type<C, int>("C");
    std::string type;
    while(true)
    {
        A* p = nullptr;
        std::cin >> type;
        if(type == "C")
        {
            int d;
            std::cin >> d;
            p = o.make_object<A>("C", d);
        }
        else
        {
            p = o.make_object<A>(type);
        }
        if (p != nullptr)
        {
            p->func();
            delete p;
        }
    }
}
