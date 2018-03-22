#include "sf_object.h"
#include <iostream>
using namespace skyfire;
using namespace std;

class A : public sf_object<>
{
public:
    SF_REG_AOP(func, int , int)
    void func(int a, int b)
    {
        cout<<a+b<<endl;
    }
};

void before1(int a, int b)
{
    cout<<"before call" <<endl;
}

void before2(int a, int b)
{
    cout<< "a = " << a <<" ,b = " << b <<endl;
}

void after()
{
    cout<<"after call"<<endl;
}

int main()
{
    A a;
    int b2 = sf_aop_before_bind(&a,func,before2);
    int b1 = sf_aop_before_bind(&a,func,before1);
    int c = sf_aop_after_bind(&a, func, after);
    a.aop_func(5,10);
    cout<<"---------"<<endl;
    sf_aop_before_unbind(&a, func, b1);
    a.aop_func(100, 200);
    cout<<"---------"<<endl;
    sf_aop_before_unbind(&a, func, b2);
    sf_aop_after_unbind(&a, func, c);
    a.aop_func(200,300);
}