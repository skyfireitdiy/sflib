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
        bool inited__ = false;
        bool raw__ = false;
        int sock__ = -1;
    public:
        sf_tcpclient(bool raw = false);

        SOCKET get_raw_socket();

        bool bind(const std::string& ip, unsigned short port);

        static std::shared_ptr<sf_tcpclient> make_client(bool raw = false);

        ~sf_tcpclient();

        bool connect(const std::string &ip, unsigned short port);


        bool send(int type, const byte_array &data);

        bool send(const byte_array &data);

        void close();
    };

}
