
/**
* @version 1.0.0
* @author skyfire
* @mail skyfireitdiy@hotmail.com
* @see http://github.com/skyfireitdiy/sflib
* @file sf_tcp_server_linux.h

* sflib第一版本发布
* 版本号1.0.0
* 发布日期：2018-10-22
*/

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
#include "sf_tcp_utils.hpp"
#include "sf_nocopy.h"
#include "sf_object.hpp"
#include "sf_type.hpp"
#include "sf_range.hpp"
#include "sf_serialize_binary.hpp"
#include "sf_logger.hpp"
#include "sf_tcp_server_interface.h"


namespace skyfire
{
    class sf_tcp_server : public sf_tcp_server_interface
    {
    private:
        int listen_fd__ = -1;
        std::map<int, byte_array> sock_data_buffer__;
        int cur_fd_count__ = -1;
        int epoll_fd__ = -1;
        bool raw__ = false;
        epoll_event evs[SOMAXCONN];
        epoll_event ev;

    public:
        sf_tcp_server(bool raw = false);

        ~sf_tcp_server();

        static std::shared_ptr<sf_tcp_server> make_server(bool raw = false);

        bool listen(const std::string &ip, unsigned short port) override;


        void close() override;
        
        void close(SOCKET sock) override;

        bool send(int sock, int type, const byte_array &data) override;

        bool send(int sock, const byte_array &data) override;

        SOCKET get_raw_socket() override;
    };

}