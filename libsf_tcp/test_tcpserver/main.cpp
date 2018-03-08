#include <iostream>
#include <memory>
#include "tcpserver.h"

void acc_cb(SOCKET sock)
{
    std::cout<<"new connection"<<std::endl;
}

void recv_cb(std::shared_ptr<tcpserver> server, SOCKET sock,const pkg_header_t& header, const bytearray_t& data)
{
    auto str = ba2str(data);
    std::cout<<"recv : "<< str << "  type : " << header.type << std::endl;
    str = "(server)" + str;
    server->send(sock, header.type +1, str2ba(str));
}

void close_cb(SOCKET sock)
{
    std::cout<<sock<<" closed"<<std::endl;
}

int main() {
    std::string ip;
    unsigned short port;
    auto server = tcpserver::make_server();
    std::cout<<"IP:"<<std::flush;
    std::cin>>ip;
    std::cout<<"port:"<<std::flush;
    std::cin>>port;
    if(!server->listen(ip,port,acc_cb,std::bind(recv_cb, server, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3),close_cb))
    {
        std::cout<<"listen error"<<std::endl;
    }
    std::cout<<"listen ok"<<std::endl;
    system("pause");
    server->close();
    return 0;
}