#include <iostream>
#include "tcpclient.h"

void recv_cb(const pkg_header_t& header ,const bytearray_t& data)
{
    std::cout<<"recv : "<< ba2str(data) << "  type : " << header.type << std::endl;
}

void close_cb()
{
    std::cout<<"closed"<<std::endl;
}

int main() {
    std::string ip;
    unsigned short port;
    auto client = tcpclient::make_client();
    std::cout<<"server IP:"<<std::flush;
    std::cin>>ip;
    std::cout<<"server port:"<<std::flush;
    std::cin>>port;
    if(!client->connect(ip,port,recv_cb,close_cb))
    {
        std::cout<<"connect error"<<std::endl;
        system("pause");
        return -1;
    }
    std::cout<<"connect ok"<<std::endl;
    while(true)
    {
        std::string data;
        std::getline(std::cin, data);
        if(data == "exit")
        {
            break;
        }
        client->send(1,str2ba(data));
    }
    client->close();
    return 0;
}