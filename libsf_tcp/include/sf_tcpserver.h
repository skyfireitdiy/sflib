#pragma once

#include <winsock2.h>
#include <string>
#include <vector>
#include <functional>
#include <memory>
#include <map>
#include "sf_tcputils.h"
#include "sf_nocopy.h"
#include "sf_object.h"

namespace skyfire
{
    class sf_tcpserver : public sf_nocopy<sf_object<>>, public std::enable_shared_from_this<sf_tcpserver>
    {
        SF_REG_SIGNAL(new_connection,SOCKET);
        SF_REG_SIGNAL(data_coming,SOCKET,const pkg_header_t&,const byte_array&);
        SF_REG_SIGNAL(closed,SOCKET);
    private:
        bool inited__ = false;
        SOCKET server_sock__ = INVALID_SOCKET;

    public:
        sf_tcpserver()
        {
            WSADATA wsa_data{};
            if (WSAStartup(MAKEWORD(2, 2), &wsa_data) != 0)
            {
                inited__ = false;
                return;
            }

            server_sock__ = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
            if (server_sock__ == INVALID_SOCKET)
            {
                inited__ = false;
                return;
            }
            inited__ = true;
        }

        static std::shared_ptr <sf_tcpserver> make_server()
        {
            return std::make_shared<sf_tcpserver>();
        }


        ~sf_tcpserver()
        {
            close();
            WSACleanup();
        }

        bool listen(const std::string &ip,
                    unsigned short port
        )
        {
            if (!inited__)
                return false;
            sockaddr_in address;
            address.sin_family = AF_INET;
            address.sin_addr.S_un.S_addr = inet_addr(ip.c_str());
            address.sin_port = htons(port);
            if (::bind(server_sock__, reinterpret_cast<const sockaddr *>(&address), sizeof(address)) == SOCKET_ERROR)
            {
                return false;
            }
            if (::listen(server_sock__, SOMAXCONN) != 0)
            {
                return false;
            }

            std::thread([=]()
                        {
                            while (true)
                            {
                                sockaddr_in address;
                                int address_len = sizeof(sockaddr_in);
                                auto sock = ::accept(server_sock__, reinterpret_cast<sockaddr *>(&address),
                                                     &address_len);
                                if (sock == INVALID_SOCKET)
                                {
                                    break;
                                }
                                std::thread([=]
                                            {
                                                new_connection(sock);
                                            }).detach();
                                std::thread([=]
                                            {
                                                byte_array recv_buffer(BUFFER_SIZE);
                                                byte_array data;
                                                pkg_header_t header;
                                                while (true)
                                                {
                                                    auto len = ::recv(sock, recv_buffer.data(), BUFFER_SIZE, 0);
                                                    if (len <= 0)
                                                    {
                                                        std::thread([=]
                                                                    {
                                                                        closed(sock);
                                                                    }).detach();
                                                        break;
                                                    }
                                                    data.insert(data.end(), recv_buffer.begin(),
                                                                recv_buffer.begin() + len);
                                                    size_t read_pos = 0;
                                                    while (data.size() - read_pos >= sizeof(pkg_header_t))
                                                    {
                                                        memmove_s(&header, sizeof(header), data.data() + read_pos,
                                                                  sizeof(header));
                                                        if (data.size() - read_pos - sizeof(header) >= header.length)
                                                        {
                                                            std::thread([=]()
                                                                        {
                                                                            data_coming(sock, header, byte_array(
                                                                                    data.begin() + read_pos +
                                                                                    sizeof(header),
                                                                                    data.begin() + read_pos +
                                                                                    sizeof(header) + header.length));
                                                                        }).detach();
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
                                            }).detach();
                            }
                        }).detach();
            return true;
        }

        void close()
        {
            if (!inited__)
                return;
            closesocket(server_sock__);
            server_sock__ = INVALID_SOCKET;
        }

        bool send(SOCKET sock, int type, const byte_array &data)
        {
            if (!inited__)
                return false;
            pkg_header_t header;
            header.type = type;
            header.length = data.size();
            auto ret = ::send(sock, make_pkg(header).data(), sizeof(header), 0);
            if (ret != sizeof(header))
                return false;
            return ::send(sock, data.data(), data.size(), 0) == data.size();
        }
    };

}