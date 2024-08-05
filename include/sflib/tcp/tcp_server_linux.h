
#pragma once

#include "sflib/core/type.h"
#include "sflib/json/json.h"
#include "sflib/object/object.h"
#include "sflib/tcp/tcp_server_interface.h"
#include "sflib/tcp/tcp_utils.h"
#include "sflib/tools/nocopy.h"
#include "sflib/tools/utils.h"

#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <shared_mutex>
#include <sys/epoll.h>
#include <sys/resource.h>
#include <sys/socket.h>
#include <sys/sysinfo.h>
#include <sys/types.h>
#include <unistd.h>

namespace skyfire
{
struct sock_data_context_t
{
    epoll_event ev {};
    byte_array data_buffer_in {};
    std::deque<byte_array> data_buffer_out {};
};
struct epoll_context_t
{
    int epoll_fd {};
    int pipe__[2];
    epoll_event pipe_event__ {};
    std::shared_mutex mu_epoll_context__;
    std::unordered_map<SOCKET, sock_data_context_t> sock_context__ {};
};
class tcp_server
    : public make_instance_t<tcp_server, tcp_server_interface>
{
private:
    std::atomic<bool> closed__ = false;
    tcp_server_opt_t config__;
    SOCKET listen_fd__;
    std::vector<std::unique_ptr<std::thread>> co_vec__;
    std::vector<epoll_context_t *> context_pool__;
    mutable std::shared_mutex mu_context_pool__;
    void work_routine__(bool listen_thread = false, SOCKET listen_fd = -1);
    bool in_dispatch__(SOCKET fd);
    bool handle_accept__();
    void handle_read__(const epoll_event &ev);
    void handle_write__(const epoll_event &ev);
    epoll_context_t &epoll_data__() const;
    epoll_context_t *find_context__(SOCKET sock) const;

public:
    SOCKET raw_socket() override;
    template <typename... Args>
    tcp_server(Args &&...args);
    ~tcp_server() override;
    bool listen(const std::string &ip, unsigned short port) override;
    void close() override;
    void close(SOCKET sock) override;
    bool send(SOCKET sock, int type, const byte_array &data) override;
    bool send(SOCKET sock, const byte_array &data) override;
    sf_error recv(SOCKET sock, byte_array &data, int length = default_buffer_size) override;
};

template <typename... Args>
tcp_server::tcp_server(Args &&...args)
{
    (args(config__), ...);
}
} // namespace skyfire