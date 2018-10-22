
/**
* @version 1.0.0
* @author skyfire
* @email skyfireitdiy@hotmail.com
* @see http://github.com/skyfireitdiy/sflib
* @file test_tcp_nat_traversal_server.cpp

* sflib第一版本发布
* 版本号1.0.0
* 发布日期：2018-10-22
*/

#define SF_DEBUG

#include "sf_tcp_nat_traversal_server.hpp"

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
        std::cout<<"监听成功"<<std::endl;
    }else{
        std::cout<<"监听失败"<<std::endl;
    }

    sf_eventloop loop;
    loop.exec();
}