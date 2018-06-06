#include "sf_tcp_nat_traversal_server.h"

#include <iostream>

using namespace std;
using namespace skyfire;

int main(){
    auto pserver = sf_tcp_nat_traversal_server::make_server();
    auto ret = pserver->listen("127.0.0.1",5642);
    if(ret){
        cout<<"监听成功"<<endl;
    }else{
        cout<<"监听失败"<<endl;
    }

    sf_eventloop loop;
    loop.exec();
}