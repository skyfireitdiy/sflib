#pragma once


#include <unistd.h>
#include <sys/types.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>


#include <string>
#include <functional>
#include <memory>
#include <cstring>
#include <thread>
#include "sf_tcputils.h"
#include "sf_object.h"
#include "sf_nocopy.h"

namespace skyfire
{
    class sf_tcpclient : public sf_nocopy<sf_object>
    {
    SF_REG_SIGNAL(data_coming, const pkg_header_t &, const byte_array &)
    SF_REG_SIGNAL(raw_data_coming, const byte_array &)
    SF_REG_SIGNAL(closed)
    private:
        bool inited__ = false;
        bool raw__ = false;
        int sock__ = -1;
    public:
        sf_tcpclient(bool raw = false)
        {
            sock__ = socket(AF_INET, SOCK_STREAM, 0);
            if (sock__ == -1)
            {
                inited__ = false;
                return;
            }
            inited__ = true;
            raw__ = raw;
        }

        SOCKET get_raw_socket()
        {
            return sock__;
        }

        static std::shared_ptr<sf_tcpclient> make_client()
        {
            return std::make_shared<sf_tcpclient>();
        }

        ~sf_tcpclient()
        {
            close();
        }

        bool connect(const std::string &ip,
                     unsigned short port)
        {
            if (!inited__)
                return false;
            sockaddr_in address;
            address.sin_family = AF_INET;
            address.sin_addr.s_addr = inet_addr(ip.c_str());
            address.sin_port = htons(port);
            if (::connect(sock__, reinterpret_cast<const sockaddr *>(&address), sizeof(address)) != 0)
            {
                return false;
            }
            std::thread([=]
                        {
                            byte_array recv_buffer(BUFFER_SIZE);
                            byte_array data;
                            pkg_header_t header;
                            while (true)
                            {
                                auto len = read(sock__, recv_buffer.data(), BUFFER_SIZE);
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
                                    raw_data_coming(byte_array(recv_buffer.begin(),recv_buffer.begin() + len));
                                }
                                else
                                {
                                    data.insert(data.end(), recv_buffer.begin(), recv_buffer.begin() + len);
                                    size_t read_pos = 0;
                                    while (data.size() - read_pos >= sizeof(pkg_header_t))
                                    {
                                        std::memmove(&header, data.data() + read_pos, sizeof(header));
                                        if(!check_header_checksum(header))
                                        {
                                            close();
                                            return;
                                        }
                                        if (data.size() - read_pos - sizeof(header) >= header.length)
                                        {
                                            std::thread([=](const pkg_header_t &header_tmp, const byte_array &pkg_data)
                                                        {
                                                            data_coming(header_tmp, pkg_data);
                                                        },
                                                        header,
                                                        byte_array(
                                                                data.begin() + static_cast<long>(read_pos) + sizeof(header),
                                                                data.begin() + static_cast<long>(read_pos) + sizeof(header)
                                                                + static_cast<long>(header.length))).detach();
                                            read_pos += sizeof(header) + header.length;
                                        } else
                                        {
                                            break;
                                        }
                                    }
                                    if (read_pos != 0)
                                    {
                                        data.erase(data.begin(), data.begin() + static_cast<long>(read_pos));
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
            auto ret = write(sock__, make_pkg(header).data(), sizeof(header));
            if (ret != sizeof(header))
                return false;
            return write(sock__, data.data(), data.size()) == static_cast<ssize_t>(data.size());
        }

        bool send(const byte_array &data)
        {
            if (!inited__)
                return false;
            return write(sock__, data.data(), data.size()) == static_cast<ssize_t>(data.size());
        }



        void close()
        {
            if (!inited__)
                return;
            ::close(sock__);
            sock__ = -1;
        }
    };
}
