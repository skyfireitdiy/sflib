#include "sf_object_factory.hpp"
#include <iostream>

using namespace std;
using namespace skyfire;

class A{
private:
    int num;
public:
    A(int n) : num(n)
    {
        cout<<"A()"<<endl;
    }

    void print()
    {
        cout<<"n is "<< num<<endl;
    }
};

class B{
public:
    B()
    {
        cout<<"B()"<<endl;
    }
    void print()
    {
        cout<<"B object"<<endl;
    }
};



int main()
{
    // 1. 获取工厂对象
    auto factory = sf_object_factory::get_instance();

    // 2. 注册带参数的工厂类型
    factory->reg_object_type<A,int>("A");
    // 3. 注册无参数工厂类型
    factory->reg_object_type<B>("B");

    // 4. 设置创建前回调
    factory->set_before_create_callback([](const std::string& type){
        cout<<"before " + type + " create"<<endl;
    });

    // 5. 设置创建后回调
    factory->set_after_create_callback([](const std::string& type){
        cout<<"after " + type + " create"<<endl;
    });

    // 6. 创建对象
    auto a_obj = factory->get_object<A>("A", 20);
    auto b_obj = factory->get_object<B>("B");

    // 7. 调用函数
    a_obj->print();
    b_obj->print();

}