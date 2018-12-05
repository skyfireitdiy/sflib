
/**
* @version 1.0.0
* @author skyfire
* @mail skyfireitdiy@hotmail.com
* @see http://github.com/skyfireitdiy/sflib
* @file test_thread_pool.cpp

* sflib第一版本发布
* 版本号1.0.0
* 发布日期：2018-10-22
*/

#include "sf_thread_pool.hpp"
#include <iostream>

using namespace skyfire;


void func()
{
    for(int i=0;i<10;++i)
    {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        std::cout<<std::this_thread::get_id()<<" "<<i<<std::endl;
    }
}

void func2(int a)
{
    for(int i=0;i<a;++i)
    {
        std::cout<<i<<std::endl;
    }
}


int main()
{
    // 1. 创建线程池
    sf_thread_pool pool4(4);
    // 2.添加任务
    pool4.add_task(std::bind(func2, 10));
    // 3.等待所有任务完成
    pool4.wait_all_task_finished();
    pool4.add_task(func2, 7);
    pool4.add_task(func);
    pool4.wait_all_task_finished();
    pool4.add_task([](int a)
                   {
                       for (auto i = 0; i < 5; ++i)
                       {
                           std::cout << "hello world" << std::endl;
                       }
                   }, 5);
    pool4.wait_all_task_finished();
}