#include "sf_object.h"
#include <iostream>

using namespace skyfire;
using namespace std;

void on_print_num(int a, double b)
{
    cout<<"int:"<<a<<endl;
    cout<<"double:"<<b<<endl;
}

void on_add_num(int a,double b)
{
    cout<<"a+b="<<a+b<<endl;
}

void on_sig1()
{
    cout<<"sig1"<<endl;
}

class A : public sf_object<>
{
    public:
    SF_REG_SIGNAL(num, int, double);
    SF_REG_SIGNAL(sig1,void);
};

int main()
{
    A a;
    sf_bind_signal(&a,num, on_print_num);
    sf_bind_signal(&a,num, on_add_num);
    sf_bind_signal(&a,sig1, on_sig1);
    sf_bind_signal(&a, num, [](int a,double b){
        cout<<"a*b="<<a*b<<endl;
    });
    a.num(5,3.6);
    a.sig1();
}