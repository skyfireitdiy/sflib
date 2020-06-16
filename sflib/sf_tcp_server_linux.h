
/**
* @version 1.0.0
* @author skyfire
* @file sf_tcp_server_linux.h
*/

#pragma once

#include "sf_object.hpp"
#include "sf_tcp_server_interface.h"
#include "sf_tcp_utils.hpp"
#include "sf_type.hpp"
#include "sf_json.hpp"
#include "sf_logger.hpp"
#include "sf_nocopy.h"
#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <memory>
#include <netinet/in.h>
#include <string>
#include <sys/epoll.h>
#include <sys/resource.h>
#include <sys/socket.h>
#include <sys/sysinfo.h>
#include <sys/types.h>
#include <unistd.h>

#include "sf_utils.h"

namespace skyfire {

struct sock_data_context_in_t {
    epoll_event ev {};
    byte_array data_buffer_in {};
};

struct sock_data_context_out_t {
    epoll_event ev {};
    std::deque<byte_array> data_buffer_out {};
};

struct epoll_context_in_t {
    int epoll_fd {};
    std::unordered_map<SOCKET, sock_data_context_in_t> sock_context_in__ {};
};

struct epoll_context_out_t {
    int epoll_fd {};

    std::shared_mutex mu_epoll_context__;
    std::unordered_map<SOCKET, sock_data_context_out_t> sock_context_out__ {};
};

class sf_tcp_server
    : public sf_make_instance_t<sf_tcp_server, sf_tcp_server_interface> {
private:
    int listen_fd__ = -1;
    bool raw__ = false;
    int thread_count__ = std::thread::hardware_concurrency() * 2 + 2;

    std::vector<std::thread> thread_vec__;

    void work_thread__(bool listen_thread = false, SOCKET listen_fd = -1);

    void write_thread__();

    bool in_dispatch__(SOCKET fd);

    bool handle_accept__();

    void handle_read__(const epoll_event& ev);

    void handle_write__(const epoll_event& ev);

    epoll_context_in_t& epoll_data_in__() const;

    epoll_context_out_t& epoll_data_out__() const;


public:
    SOCKET raw_socket() override;

    explicit sf_tcp_server(bool raw = false, int thread_count = std::thread::hardware_concurrency() * 2 + 2);

    ~sf_tcp_server() override;

    bool listen(const std::string& ip, unsigned short port) override;

    void close() override;

    void close(SOCKET sock) override;

    bool send(SOCKET sock, int type, const byte_array& data) override;

    bool send(SOCKET sock, const byte_array& data) override;
};

} // namespace skyfire