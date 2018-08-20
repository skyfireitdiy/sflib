#define SF_DEBUG

#include "sf_tcp_nat_traversal_server.h"

#include <iostream>

using namespace skyfire;

int main(){
    g_logger->add_level_stream(SF_DEBUG_LEVEL,&std::cout);
    auto pserver = sf_tcp_nat_traversal_server::make_server();
    std::cout<<"port:"<<std::flush;
    unsigned short port;
    std::cin>>port;
    auto ret = pserver->listen("0.0.0.0",port);
    if(ret){
        std::cout<<"监听成功"<<endl;
    }else{
        std::cout<<"监听失败"<<endl;
    }

    sf_eventloop loop;
    loop.exec();
}