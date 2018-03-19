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
    class sf_tcpclient : public sf_nocopy<sf_object<>>, public std::enable_shared_from_this<sf_tcpclient>
    {
    SF_REG_SIGNAL(connected);
    SF_REG_SIGNAL(data_coming, const pkg_header_t &, const byte_array &);
    SF_REG_SIGNAL(closed);
    private:
        bool inited__ = false;
        int sock__ = -1;
    public:
        sf_tcpclient()
        {
            sock__ = socket(AF_INET, SOCK_STREAM, 0);
            if (sock__ == -1)
            {
                inited__ = false;
                return;
            }
            inited__ = true;
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
                                data.insert(data.end(), recv_buffer.begin(), recv_buffer.begin() + len);
                                size_t read_pos = 0;
                                while (data.size() - read_pos >= sizeof(pkg_header_t))
                                {
                                    std::memmove(&header, data.data() + read_pos, sizeof(header));
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
                                    } else
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
            return true;
        }


        bool send(int type, const byte_array &data)
        {
            if (!inited__)
                return false;
            pkg_header_t header;
            header.type = type;
            header.length = data.size();
            auto ret = write(sock__, make_pkg(header).data(), sizeof(header));
            if (ret != sizeof(header))
                return false;
            return write(sock__, data.data(), data.size()) == data.size();
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
