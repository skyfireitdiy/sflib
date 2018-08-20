#include "sf_thread_pool.h"
#include <iostream>
#include <chrono>
#include <functional>

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
    sf_thread_pool pool4(4);
    pool4.add_task(std::bind(func2, 10));
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