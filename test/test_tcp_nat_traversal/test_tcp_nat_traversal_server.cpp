#define SF_DEBUG

#include "sf_tcp_nat_traversal_server.h"

#include <iostream>

using namespace std;
using namespace skyfire;

int main(){
    __logger__->add_level_stream(SF_DEBUG_LEVEL,&cout);
    auto pserver = sf_tcp_nat_traversal_server::make_server();
    cout<<"port:"<<flush;
    unsigned short port;
    cin>>port;
    auto ret = pserver->listen("0.0.0.0",port);
    if(ret){
        cout<<"监听成功"<<endl;
    }else{
        cout<<"监听失败"<<endl;
    }

    sf_eventloop loop;
    loop.exec();
}