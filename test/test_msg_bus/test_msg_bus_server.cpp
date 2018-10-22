
/**
* @version 1.0.0
* @author skyfire
* @email skyfireitdiy@hotmail.com
* @see http://github.com/skyfireitdiy/sflib
* @file test_msg_bus_server.cpp

* sflib第一版本发布
* 版本号1.0.0
* 发布日期：2018-10-22
*/

#include <iostream>
#include "sf_msg_bus_server.hpp"

using namespace skyfire;

int main()
{
    auto server = sf_msg_bus_server::make_server();
    server->listen("127.0.0.1", 5678);
    std::string type;
    std::string data;
    while(true)
    {
        std::cout<<"type:"<<std::flush;
        std::cin>>type;
        if(type == "quit")
            break;
        std::cout<<"data:"<<std::flush;
        std::cin>>data;
        server->send_msg(type, sf_serialize_binary(data));
    }
    server->close();
}