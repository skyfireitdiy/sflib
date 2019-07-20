
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
#include "sf_json.hpp"
#include "sf_logger.hpp"
#include "sf_tcp_server_interface.h"
#include <sys/sysinfo.h>

#include "sf_utils.h"


namespace skyfire
{

    struct sock_data_context_t{
        epoll_event ev;
        byte_array data_buffer_in;
        std::shared_ptr<std::mutex> mu_out_buffer = std::make_shared<std::mutex>();
        std::deque<byte_array> data_buffer_out;
    };

    struct epoll_context_t{
        int epoll_fd;
        // FIXME 此处应该加个锁，对sock_context__的访问在高并发环境下有冲突
        std::unordered_map<SOCKET, sock_data_context_t> sock_context__;
    };

    class sf_tcp_server : public sf_make_instance_t<sf_tcp_server ,sf_tcp_server_interface>
    {
    private:
        int listen_fd__ = -1;
        bool raw__ = false;
		int thread_count__ = std::thread::hardware_concurrency() * 2 + 2;

        std::vector<epoll_context_t> epoll_data__;

		std::vector<std::thread> thread_vec__;

        void work_thread(int index, bool listen_thread = false, SOCKET listen_fd = -1);

        bool in_dispatch(SOCKET fd);

        int find_fd_epoll_index(SOCKET fd);

        bool handle_accept(int index);

        void handle_read(int index,const  epoll_event &ev);

        void handle_write(int index, const epoll_event &ev);

    public:
        SOCKET get_raw_socket() override;

        explicit sf_tcp_server(bool raw = false);

        ~sf_tcp_server() override;

        bool listen(const std::string &ip, unsigned short port) override;

        void close() override;

        void close(SOCKET sock) override;

        bool send(SOCKET sock, int type, const byte_array &data) override;

        bool send(SOCKET sock, const byte_array &data) override;

        bool detach(SOCKET sock) override ;

    };

}