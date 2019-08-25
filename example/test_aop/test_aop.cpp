
/**
* @version 1.0.0
* @author skyfire
* @mail skyfireitdiy@hotmail.com
* @see http://github.com/skyfireitdiy/sflib
* @file test_aop.cpp

* sflib第一版本发布
* 版本号1.0.0
* 发布日期：2018-10-22
*/

#include <core/sf_object.hpp>
using namespace skyfire;

// 1.对象继承自sf_object
class A : public sf_object {
   public:
    // 2. 注册aop成员函数，会生成aop_xxx函数
    SF_REG_AOP(func, int, int)
    int func(int a, int b) const {
        std::cout << a + b << std::endl;
        return a + b;
    }
};

// 3.定义一个函数，使其插入到函数调用前，参数列表与要注入的函数相同
void before_call(int a, int b) {
    std::cout << "a=" << a << ",b=" << b << std::endl;
}

// 4.定义一个函数，使其插入到函数调用后，参数列表与要注入的函数返回类型
void after_call() { std::cout << "call finished" << std::endl; }

int main() {
    A a;
    // 5.注入before_call，参数为 对象指针、成员函数名称、注入函数名称,
    // 返回注入id，可用此id撤销注入
    auto id = sf_aop_before_bind(&a, func, before_call);
    // 6. 同样也支持lambda
    sf_aop_before_bind(&a, func, [](int a, int b) {
        std::cout << "this is lambda, a=" << a << ",b=" << b << std::endl;
    });
    // 7. 注入after_call
    sf_aop_after_bind(&a, func, after_call);
    // 8. 调用函数
    a.aop_func(5, 10);

    // 输出:
    //    this is lambda, a=5,b=10
    //    a=5,b=10
    //    15
    //    call finished

    // 9.撤销注入
    sf_aop_before_unbind(&a, func, id);
    a.aop_func(5, 10);

    // 输出：
    //    this is lambda, a=5,b=10
    //    15
    //    call finished
}