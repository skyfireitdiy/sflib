#include "../include/tcpserver.h"
#include <thread>

namespace skyfire
{

tcpserver::tcpserver()
{
    WSADATA wsa_data{};
    if(WSAStartup(MAKEWORD(2,2), &wsa_data) != 0)
    {
        inited__ = false;
        return ;
    }

    server_sock__ = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (server_sock__ == INVALID_SOCKET)
    {
        inited__ = false;
        return;
    }
    inited__ = true;
}

bool tcpserver::listen(const std::string &ip,
                       unsigned short port, 
                       std::function<void(SOCKET)> accept_callback,
                       std::function<void(SOCKET, const pkg_header_t&, const bytearray_t &)> receive_callback,
                       std::function<void(SOCKET)> close_callback) {
    if(!inited__)
        return false;
    sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.S_un.S_addr = inet_addr(ip.c_str());
    address.sin_port = htons(port);
    if(::bind(server_sock__, reinterpret_cast<const sockaddr*>(&address), sizeof(address)) == SOCKET_ERROR)
    {
        return false;
    }
    if(::listen(server_sock__, SOMAXCONN) != 0)
    {
        return false;
    }

    std::thread([=](){
        while(true)
        {
            sockaddr_in address;
            int address_len = sizeof(sockaddr_in);
            auto sock = ::accept(server_sock__, reinterpret_cast<sockaddr*>(&address), &address_len);
            if(sock == INVALID_SOCKET)
            {
                break;
            }
            std::thread([=]{
                accept_callback(sock);
            }).detach();
            std::thread([=]{
                bytearray_t recv_buffer(BUFFER_SIZE);
                bytearray_t data;
                pkg_header_t header;
                while(true)
                {
                    auto len = ::recv(sock, recv_buffer.data(), BUFFER_SIZE, 0);
                    if(len <= 0)
                    {
                        std::thread([=]{
                            close_callback(sock);
                        }).detach();
                        break;
                    }
                    data.insert(data.end(),recv_buffer.begin(),recv_buffer.begin() + len);
                    size_t read_pos = 0;
                    while(data.size() - read_pos >= sizeof(pkg_header_t))
                    {
                        memmove_s(&header, sizeof(header), data.data()+read_pos, sizeof(header));
                        if(data.size()-read_pos-sizeof(header) >= header.length)
                        {
                            std::thread([=](){
                                receive_callback(sock ,header, bytearray_t(data.begin()+read_pos+sizeof(header),data.begin()+read_pos+sizeof(header)+header.length));
                            }).detach();
                            read_pos += sizeof(header)+header.length;
                        } 
                        else
                        {
                            break;
                        }
                    }
                    if(read_pos!=0)
                    {
                        data.erase(data.begin(), data.begin()+read_pos);
                    }
                }
            }).detach();
        }
    }).detach();
    return true;
}

void tcpserver::close()
{
    if(!inited__)
        return;
    closesocket(server_sock__);
    server_sock__ = INVALID_SOCKET;
}

bool tcpserver::send(SOCKET sock, int type, const bytearray_t &data) {
    if(!inited__)
        return false;
    pkg_header_t header;
    header.type = type;
    header.length = data.size();
    auto ret = ::send(sock, make_pkg(header).data(), sizeof(header), 0);
    if(ret != sizeof(header))
        return false;
    return ::send(sock, data.data(), data.size(), 0) == data.size();
}

tcpserver::~tcpserver()
{
    close();
    WSACleanup();
}


std::shared_ptr<tcpserver> tcpserver::make_server()
{
    return std::make_shared<tcpserver>();
}

}