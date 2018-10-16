#include <iostream>
#include "sf_msg_bus_client.hpp"

using namespace skyfire;

int main()
{
    auto client = sf_msg_bus_client::make_client();
    client->connect_to_server("127.0.0.1", 5678);
    sf_bind_signal(client, msg_come, [](std::string, byte_array data){
        std::string str;
        sf_deserialize_binary(data, str, 0);
        std::cout<<"msg_come:"<<str<<std::endl;
        }, true);
    std::string type;
    std::cin>>type;
    client->reg_msg_to_bus(type);
    sf_eventloop e;
    e.exec();
}