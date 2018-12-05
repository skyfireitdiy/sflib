
/**
* @version 1.0.0
* @author skyfire
* @mail skyfireitdiy@hotmail.com
* @see http://github.com/skyfireitdiy/sflib
* @file test_finally.cpp

* sflib第一版本发布
* 版本号1.0.0
* 发布日期：2018-10-22
*/

#include "sf_finally.hpp"
#include <iostream>

using namespace skyfire;
using namespace std;

int main()
{
    int *p = new int;
    // 1. 注册一个删除函数，在作用域结束的时候执行（后执行）
    sf_finally del_p([&](){
        delete p;
        cout<<"Delete p"<<endl;
    });
    {
        int *q = new int;
        // 2. 注册一个删除函数，在作用域结束的时候执行（先执行）
        sf_finally del_q([&](){
            delete q;
            cout<<"Delete q"<<endl;
        });
    }
}