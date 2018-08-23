#pragma once

#include <winsock2.h>
#include <string>
#include <functional>
#include <memory>
#include <thread>
#include "sf_tcputils.hpp"
#include "sf_object.hpp"
#include "sf_nocopy.hpp"

namespace skyfire
{
    class sf_tcpclient : public sf_nocopy<sf_object>
    {
        SF_REG_SIGNAL(data_coming, const pkg_header_t &, const byte_array &)
        SF_REG_SIGNAL(raw_data_coming, const byte_array &)
        SF_REG_SIGNAL(closed)
    private:
        bool raw__ = false;
        bool inited__ = false;
        SOCKET sock__ = INVALID_SOCKET;
    public:

        SOCKET get_raw_socket();

        bool bind(const std::string& ip, unsigned short port);

        sf_tcpclient(bool raw = false);

        static std::shared_ptr <sf_tcpclient> make_client(bool raw = false);

        ~sf_tcpclient();

        bool connect(const std::string &ip, unsigned short port);


        bool send(int type, const byte_array &data);

        bool send(const byte_array & data);

        void close();
    };

}
