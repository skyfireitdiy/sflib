#include <iostream>
#include "sf_msg_bus_server.h"

using namespace skyfire;
using namespace std;

int main()
{
    auto server = sf_msg_bus_server::make_server();
    server->listen("127.0.0.1", 5678);
    string type;
    string data;
    while(true)
    {
        cout<<"type:"<<flush;
        cin>>type;
        if(type == "quit")
            break;
        cout<<"data:"<<flush;
        cin>>data;
        server->send_msg(type, sf_serialize_binary(data));
    }
    server->close();
}