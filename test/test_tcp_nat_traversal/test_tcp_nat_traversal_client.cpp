#include "sf_tcp_nat_traversal_client.h"
#include <iostream>
#include <sf_timer.h>

#include "sf_random.h"
#include "sf_range.h"

using namespace skyfire;
using namespace std;

int main() {

    for(auto i: sf_range(5)) {
        cout<<sf_random::get_instance()->get_int(0, 5)<<endl;
        cout<<sf_random::get_instance()->get_double(0.5, 9.8)<<endl;

    }

//    auto pclient = sf_tcp_nat_traversal_client::make_client();
//    auto ret = pclient->connect("127.0.0.1", 5642);
//    if (ret) {
//        cout << "connected" << endl;
//    } else {
//        cout << "error" << endl;
//    }
//
//    getchar();
//
//
//    sf_eventloop loop;
//    loop.exec();
}