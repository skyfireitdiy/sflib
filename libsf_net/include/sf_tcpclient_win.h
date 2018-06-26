#pragma once

#include <winsock2.h>
#include <string>
#include <functional>
#include <memory>
#include <thread>
#include "sf_tcputils.h"
#include "sf_object.h"
#include "sf_nocopy.h"

namespace skyfire
{
    class sf_tcpclient : public sf_nocopy<sf_object>
    {
        SF_REG_SIGNAL(data_coming, const pkg_header_t &, const byte_array &);
        SF_REG_SIGNAL(raw_data_coming, const byte_array &);
        SF_REG_SIGNAL(closed);
    private:
        bool raw__ = false;
        bool inited__ = false;
        SOCKET sock__ = INVALID_SOCKET;
    public:

        SOCKET get_raw_socket()
        {
            return sock__;
        }

        bool bind(const std::string& ip, unsigned short port){
            sockaddr_in address;
            address.sin_family = AF_INET;
            address.sin_addr.S_un.S_addr = inet_addr(ip.c_str());
            address.sin_port = htons(port);
            return SOCKET_ERROR != ::bind(sock__,reinterpret_cast<sockaddr*>(&address), sizeof(address));
        }

        sf_tcpclient(bool raw = false)
        {
            WSADATA wsa_data{};
            if (WSAStartup(MAKEWORD(2, 2), &wsa_data) != 0)
            {
                inited__ = false;
                return;
            }

            sock__ = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
            if (sock__ == INVALID_SOCKET)
            {
                inited__ = false;
                return;
            }

            int op = 1;
            if(SOCKET_ERROR == setsockopt(sock__,
                                          SOL_SOCKET,
                                          SO_REUSEADDR,
                                          reinterpret_cast<char*>(&op),
                                          sizeof(op))){
                inited__ = false;
                return;
            }

            inited__ = true;
            raw__ = raw;
        }


        static std::shared_ptr <sf_tcpclient> make_client(bool raw = false)
        {
            return std::make_shared<sf_tcpclient>(raw);
        }

        ~sf_tcpclient()
        {
            close();
            WSACleanup();
        }

        bool connect(const std::string &ip,
                     unsigned short port)
        {
            if (!inited__)
                return false;
            sockaddr_in address;
            address.sin_family = AF_INET;
            address.sin_addr.S_un.S_addr = inet_addr(ip.c_str());
            address.sin_port = htons(port);
            if (::connect(sock__, reinterpret_cast<const sockaddr *>(&address), sizeof(address)) != 0)
            {
                return false;
            }
            std::thread([=]
                        {
                            byte_array recv_buffer(SF_NET_BUFFER_SIZE);
                            byte_array data;
                            pkg_header_t header;
                            while (true)
                            {
                                auto len = ::recv(sock__, recv_buffer.data(), SF_NET_BUFFER_SIZE, 0);
                                if (len <= 0)
                                {
                                    std::thread([=]
                                                {
                                                    closed();
                                                }).detach();
                                    break;
                                }
                                if(raw__)
                                {
                                    raw_data_coming(byte_array(recv_buffer.begin(),recv_buffer.begin()+len));
                                }
                                else
                                {
                                    data.insert(data.end(), recv_buffer.begin(), recv_buffer.begin() + len);
                                    size_t read_pos = 0;
                                    while (data.size() - read_pos >= sizeof(pkg_header_t))
                                    {
                                        memmove_s(&header, sizeof(header), data.data() + read_pos, sizeof(header));
                                        if(!check_header_checksum(header))
                                        {
                                            close();
                                            return;
                                        }
                                        if (data.size() - read_pos - sizeof(header) >= header.length)
                                        {
                                            std::thread([=](const pkg_header_t &header, const byte_array &pkg_data)
                                                        {
                                                            data_coming(header, pkg_data);
                                                        },
                                                        header,
                                                        byte_array(
                                                                data.begin() + read_pos + sizeof(header),
                                                                data.begin() + read_pos + sizeof(header)
                                                                + header.length)).detach();
                                            read_pos += sizeof(header) + header.length;
                                        }
                                        else
                                        {
                                            break;
                                        }
                                    }
                                    if (read_pos != 0)
                                    {
                                        data.erase(data.begin(), data.begin() + read_pos);
                                    }
                                }
                            }
                        }).detach();
            return true;
        }


        bool send(int type, const byte_array &data)
        {
            if (!inited__)
                return false;
            pkg_header_t header;
            header.type = type;
            header.length = data.size();
            make_header_checksum(header);
            auto ret = ::send(sock__, make_pkg(header).data(), sizeof(header), 0);
            if (ret != sizeof(header))
                return false;
            return ::send(sock__, data.data(), data.size(), 0) == data.size();
        }

        bool raw_send(const byte_array & data)
        {
            if (!inited__)
                return false;
            return ::send(sock__, data.data(), data.size(), 0) == data.size();
        }

        void close()
        {
            if (!inited__)
                return;
            shutdown(sock__,SD_BOTH);
            closesocket(sock__);
            sock__ = INVALID_SOCKET;
        }
    };

}
