
/**
* @version 1.0.0
* @author skyfire
* @file test_object.cpp
*/

#include "sf_object"

using namespace skyfire;

// 1. 类继承自sf_object
class A : public sf_object {
    // 2.注册一个信号，信号名称为s1，参数列表为int
    SF_REG_SIGNAL(s1, int)
};

// 3.定义一个槽函数，参数列表与信号参数列表一致
void slot1(int a) {
    std::cout << std::this_thread::get_id() << " " << a << std::endl;
}

int main() {
    A a;
    // 4.绑定信号与槽，
    // 对象指针（可以是智能指针），信号名称，槽，是否使用消息队列
    sf_bind_signal(&a, s1, slot1, false);
    sf_bind_signal(&a, s1, slot1, true);
    a.s1(56);

    sf_eventloop e;
    // 5.启动事件循环
    e.exec();
}