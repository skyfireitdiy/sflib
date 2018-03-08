//
// Created by wangmaobin on 2018/3/7.
//

#include <inaddr.h>
#include "../include/tcpclient.h"
#include <thread>

namespace skyfire
{

tcpclient::tcpclient()
{
    WSADATA wsa_data{};
    if(WSAStartup(MAKEWORD(2,2), &wsa_data) != 0)
    {
        inited__ = false;
        return ;
    }

    sock__ = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock__ == INVALID_SOCKET)
    {
        inited__ = false;
        return;
    }
    inited__ = true;
}

tcpclient::~tcpclient()
{
    close();
    WSACleanup();
}

void tcpclient::close()
{
    if(!inited__)
        return;
    closesocket(sock__);
    sock__ = INVALID_SOCKET;
}

bool tcpclient::connect(const std::string &ip, unsigned short port, std::function<void(const pkg_header_t &,const bytearray_t&)> receive_callback, 
                        std::function<void()> close_callback) {
    if(!inited__)
        return false;
    sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.S_un.S_addr = inet_addr(ip.c_str());
    address.sin_port = htons(port);
    if(::connect(sock__, reinterpret_cast<const sockaddr*>(&address), sizeof(address)) != 0)
    {
        return false;
    }
    std::thread([=]{
        bytearray_t recv_buffer(BUFFER_SIZE);
        bytearray_t data;
        pkg_header_t header;
        while(true)
        {
            auto len = ::recv(sock__, recv_buffer.data(), BUFFER_SIZE, 0);
            if(len <= 0)
            {
                std::thread([=]{
                    close_callback();
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
                        receive_callback(header, bytearray_t(data.begin()+read_pos+sizeof(header),data.begin()+read_pos+sizeof(header)+header.length));
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
    return true;
}

bool tcpclient::send(int type,const bytearray_t &data) {
    if(!inited__)
        return false;
    pkg_header_t header;
    header.type = type;
    header.length = data.size();
    auto ret = ::send(sock__, make_pkg(header).data(), sizeof(header), 0);
    if(ret != sizeof(header))
        return false;
    return ::send(sock__, data.data(), data.size(), 0) == data.size();
}

std::shared_ptr<tcpclient> tcpclient::make_client()
{
    return std::make_shared<tcpclient>();
}

}