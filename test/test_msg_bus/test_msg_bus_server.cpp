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