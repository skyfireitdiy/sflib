#pragma once

#include "sf_nocopy.h"
#include "sf_object.h"
#include "sf_type.h"
#include "sf_tcputils.h"
#include <string>

namespace skyfire
{
    class sf_tcp_server_interface : public sf_nocopy<sf_object>
    {
    SF_REG_SIGNAL(new_connection, SOCKET);
    SF_REG_SIGNAL(data_coming, SOCKET, const pkg_header_t&, const byte_array&);
    SF_REG_SIGNAL(raw_data_coming, SOCKET, const byte_array&);
    SF_REG_SIGNAL(closed, SOCKET);
    public:
        virtual SOCKET get_raw_socket() = 0;
        virtual bool listen(const std::string &ip, unsigned short port) = 0;
        virtual void close() = 0;
        virtual void close(SOCKET sock) = 0;
        virtual bool send(SOCKET sock, int type, const byte_array &data) = 0;
        virtual bool send(SOCKET sock, const byte_array &data) = 0;
    };
}