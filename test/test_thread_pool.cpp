#include "sf_thread_pool.h"
#include <iostream>
#include <chrono>
#include <functional>

using namespace skyfire;
using namespace std;


void func()
{
    for(int i=0;i<10;++i)
    {
        this_thread::sleep_for(chrono::seconds(1));
        cout<<this_thread::get_id()<<" "<<i<<endl;
    }
}

void func2(int a)
{
    for(int i=0;i<a;++i)
    {
        cout<<i<<endl;
    }
}


int main()
{
    sf_thread_pool pool4(4);
    pool4.add_task(bind(func2,10));
    pool4.wait_all_task_finished();
    pool4.add_task(func2, 7);
    pool4.add_task(func);
    pool4.wait_all_task_finished();

}