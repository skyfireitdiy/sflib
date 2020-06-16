
/**
* @version 1.0.0
* @author skyfire
* @file test_chan.cpp

* 
*/

//
// Created by skyfire on 19-9-25.
//

#include "sf_chan"
#include "sf_thread_pool"
#include <iostream>

using namespace std;
using namespace skyfire;

void product(const shared_ptr<sf_chan<int>>& ch)
{
    for (int i = 0; i < 100; ++i) {
        cout << "thread:" << this_thread::get_id() << " product:" << i << endl;
        i >> ch;
    }
    ch->close();
}

void custom(const shared_ptr<sf_chan<int>>& ch)
{
    try {
        while (true) {
            int i;
            ch >> i;
            cout << "\t\tthread:" << this_thread::get_id() << " custom:" << i << endl;
        }
    } catch (sf_exception& e) {
        cout << e.what() << endl;
    }
}

int main()
{
    auto pool = sf_thread_pool::make_instance(32);
    auto ch = sf_chan<int>::make_instance(5);
    pool->add_task(product, ch);
    pool->add_task(product, ch);
    pool->add_task(custom, ch);
    pool->wait_all_task_finished();
}