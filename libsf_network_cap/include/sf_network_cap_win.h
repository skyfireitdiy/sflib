#pragma once
#include <atomic>
#include <winsock2.h>
#include "sf_object.h"
#include "sf_type.h"
#include "sf_network_utils.h"
#include <mstcpip.h>
#include <ws2tcpip.h>

#include <thread>

namespace skyfire
{
    class sf_network_cap: public sf_object<sf_nocopy<>>
    {
    SF_REG_SIGNAL(data_caped, const byte_array& data)
    SF_REG_SIGNAL(error)

    private:
        SOCKET sock__ = INVALID_SOCKET;
    public:
        ~sf_network_cap()
        {
            WSACleanup();
        }

        bool start(const std::string &ip)
        {
            WSADATA wsa_data{};
            if (WSAStartup(MAKEWORD(2, 2), &wsa_data) != 0)
            {
                return false;
            }
            sock__ = socket(AF_INET, SOCK_RAW, IPPROTO_IP);
            if (sock__ == INVALID_SOCKET)
            {
                return false;
            }
            sockaddr_in addr;
            addr.sin_family = AF_INET;
            addr.sin_port = 0;
            addr.sin_addr.S_un.S_addr = inet_addr(ip.c_str());

            if (::bind(sock__, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) == SOCKET_ERROR)
            {
                return false;
            }
            int flag = 1;
            DWORD out_buffer[10];
            DWORD in_buffer = 1;
            DWORD return_len = 0;

            if(WSAIoctl(sock__,SIO_RCVALL,&in_buffer, sizeof(in_buffer), out_buffer, sizeof(out_buffer), &return_len,
                        nullptr, 0) !=0 )
            {
                return false;
            }


            std::thread([=]()
                        {
                            byte_array buffer(NETWORK_RAW_BUFFER_SIZE);
                            while (true)
                            {
                                buffer.resize(NETWORK_RAW_BUFFER_SIZE);
                                int ret_num = recv(sock__, buffer.data(), NETWORK_RAW_BUFFER_SIZE, 0);
                                if (ret_num > 0)
                                {
                                    buffer.resize(ret_num);
                                    data_caped(buffer);
                                }
                                else
                                {
                                    error();
                                    break;
                                }
                            }
                        }).detach();

            return true;
        }

        void stop()
        {
            if(sock__ != INVALID_SOCKET)
            {
                closesocket(sock__);
            }
        }
    };
}
