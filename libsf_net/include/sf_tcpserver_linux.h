#pragma once

#include <unistd.h>
#include <sys/types.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <sys/resource.h>
#include <memory>
#include <string>
#include "sf_tcputils.hpp"
#include "sf_nocopy.hpp"
#include "sf_object.hpp"
#include "sf_type.hpp"
#include "sf_range.hpp"
#include "sf_serialize_binary.hpp"
#include "sf_logger.hpp"


namespace skyfire
{
    class sf_tcpserver : public sf_nocopy<sf_object>
    {

    SF_REG_SIGNAL(new_connection, SOCKET)
    SF_REG_SIGNAL(data_coming, SOCKET, const pkg_header_t&, const byte_array&)
    SF_REG_SIGNAL(raw_data_coming, SOCKET, const byte_array&)
    SF_REG_SIGNAL(closed, SOCKET)

    private:
        int listen_fd__ = -1;
        std::map<int, byte_array> sock_data_buffer__;
        int cur_fd_count__ = -1;
        int epoll_fd__ = -1;
        bool raw__ = false;
        epoll_event evs[SOMAXCONN];
        epoll_event ev;

    public:
        sf_tcpserver(bool raw = false);

        ~sf_tcpserver();

        static std::shared_ptr<sf_tcpserver> make_server(bool raw = false);

        bool listen(const std::string &ip, unsigned short port);


        void close();
        
        void close(SOCKET sock);

        bool send(int sock, int type, const byte_array &data);

        bool send(int sock, const byte_array &data);

        SOCKET get_raw_socket();
    };

}