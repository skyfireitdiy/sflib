#include "sf_meta_object.h"
#include <string>
#include <iostream>
using namespace skyfire;
using namespace std;
struct A
{
    virtual void func() { cout<<"class A"<<endl; }
};
struct B: public A
{
    virtual void func() { cout<<"class B"<<endl; }
};
struct C: public A
{
    int data;
    C(int i) : data(i) {}
    virtual void func() { cout<<"class C, data:"<<data<<endl; }
};

int main()
{
    sf_meta_object o;
    o.reg_meta_type<A>("A");
    o.reg_meta_type<B>("B");
    o.reg_meta_type<C, int>("C");
    string type;
    while(true)
    {
        A* p = nullptr;
        cin >> type;
        if(type == "C")
        {
            int d;
            cin >> d;
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
