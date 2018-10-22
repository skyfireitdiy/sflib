
/**
* @version 1.0.0
* @author skyfire
* @email skyfireitdiy@hotmail.com
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
    sf_finally del_p([&](){
        delete p;
        cout<<"Delete p"<<endl;
    });
    {
        int *q = new int;
        sf_finally del_q([&](){
            delete q;
            cout<<"Delete q"<<endl;
        });
    }
}