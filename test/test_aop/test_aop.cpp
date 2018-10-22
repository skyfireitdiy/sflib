
/**
* @version 1.0.0
* @author skyfire
* @email skyfireitdiy@hotmail.com
* @see http://github.com/skyfireitdiy/sflib
* @file test_aop.cpp

* sflib第一版本发布
* 版本号1.0.0
* 发布日期：2018-10-22
*/

#include "sf_object.hpp"
#include <iostream>
using namespace skyfire;

class A : public sf_object
{
public:
    SF_REG_AOP(func, int , int)
    void func(int a, int b)
    {
        std::cout<<a+b<<std::endl;
    }
};

void before1(int a, int b)
{
    std::cout<<"before call" <<std::endl;
}

void before2(int a, int b)
{
    std::cout<< "a = " << a <<" ,b = " << b <<std::endl;
}

void after()
{
    std::cout<<"after call"<<std::endl;
}

int main()
{
    A a;
    int b2 = sf_aop_before_bind(&a,func,before2);
    int b1 = sf_aop_before_bind(&a,func,before1);
    int c = sf_aop_after_bind(&a, func, after);
    a.aop_func(5,10);
    std::cout<<"---------"<<std::endl;
    sf_aop_before_unbind(&a, func, b1);
    a.aop_func(100, 200);
    std::cout<<"---------"<<std::endl;
    sf_aop_before_unbind(&a, func, b2);
    sf_aop_after_unbind(&a, func, c);
    a.aop_func(200,300);
}