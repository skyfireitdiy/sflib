
/**
* @version 1.0.0
* @author skyfire
* @email skyfireitdiy@hotmail.com
* @see http://github.com/skyfireitdiy/sflib
* @file test_object.cpp

* sflib第一版本发布
* 版本号1.0.0
* 发布日期：2018-10-22
*/

#include "sf_object.hpp"
#include <iostream>
#include <thread>

using namespace skyfire;

class A: public sf_object
{
    SF_REG_SIGNAL(s1, int)
};

void slot1(int a)
{
    std::cout<<std::this_thread::get_id()<<" "<<a<<std::endl;
}

int main()
{
    A a;
    std::cout<<std::this_thread::get_id()<<std::endl;
    sf_bind_signal(&a, s1, slot1, false);
    sf_bind_signal(&a, s1, slot1, true);
    a.s1(56);

    sf_eventloop e;
    e.exec();
}