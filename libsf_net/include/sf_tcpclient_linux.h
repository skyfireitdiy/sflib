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

#include "sf_tcpclient_interface.h"


namespace skyfire
{
    class sf_tcpclient : public sf_tcpclient_interface
    {
    private:
        bool inited__ = false;
        bool raw__ = false;
        int sock__ = -1;
    public:
        sf_tcpclient(bool raw = false);

        SOCKET get_raw_socket() override;

        bool bind(const std::string& ip, unsigned short port) override;

        static std::shared_ptr<sf_tcpclient> make_client(bool raw = false);

        ~sf_tcpclient();

        bool connect(const std::string &ip, unsigned short port) override;


        bool send(int type, const byte_array &data) override;

        bool send(const byte_array &data) override;

        void close() override;
    };

}
