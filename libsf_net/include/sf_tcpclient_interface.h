#pragma once

#include "sf_tcputils.h"
#include "sf_object.h"
#include "sf_nocopy.h"
#include "sf_type.h"
#include <string>

namespace skyfire
{
    class sf_tcpclient_interface : public sf_nocopy<sf_object>
    {
        SF_REG_SIGNAL(data_coming, const pkg_header_t &, const byte_array &)
        SF_REG_SIGNAL(raw_data_coming, const byte_array &)
        SF_REG_SIGNAL(closed)
    public:
        virtual SOCKET get_raw_socket() = 0;

        virtual bool bind(const std::string& ip, unsigned short port) = 0;

        virtual bool connect(const std::string &ip, unsigned short port) = 0;

        virtual bool send(int type, const byte_array &data) = 0;

        virtual bool send(const byte_array & data) = 0;

        virtual void close() = 0;
    };
}