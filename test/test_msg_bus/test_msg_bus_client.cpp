#include <iostream>
#include "sf_msg_bus_client.h"

using namespace skyfire;
using namespace std;

int main()
{
    auto client = sf_msg_bus_client::make_client();
    client->connect_to_bus("127.0.0.1", 5678);
    sf_bind_signal(client, msg_come, [](std::string, byte_array data){
        string str;
        sf_deserialize(data, str, 0);
        cout<<"msg_come:"<<str<<endl;
        }, true);
    string type;
    cin>>type;
    client->reg_msg_to_bus(type);
    sf_eventloop e;
    e.exec();
}