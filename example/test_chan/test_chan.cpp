//
// Created by skyfire on 19-9-25.
//

#include <iostream>
#include "core/sf_chan.hpp"
#include "tools/sf_thread_pool.hpp"

using namespace std;
using namespace skyfire;

void product(const shared_ptr<sf_chan<int>>& ch){
    for(int i=0;i<100;++i){
        cout<<"thread:"<<this_thread::get_id()<<" product:"<<i<<endl;
        ch<<i;
    }
    ch->close();
}

void custom(const shared_ptr<sf_chan<int>>& ch){
    try {
        while (true) {
            int i;
            i << ch;
            cout << "\t\tthread:" << this_thread::get_id() << " custom:" << i << endl;
        }
    }catch (sf_chan_close_exception &e){
        cout<<e.what()<<endl;
    }
}


int main(){
    auto pool = sf_thread_pool::make_instance(32);
    auto ch = sf_chan<int>::make_instance(5);
    pool->add_task(product, ch);
    pool->add_task(product, ch);
    pool->add_task(custom, ch);
    pool->wait_all_task_finished();
}