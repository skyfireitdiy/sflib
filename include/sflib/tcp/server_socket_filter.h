
#pragma once
#include "sflib/core/type.h"
#include "sflib/tcp/tcp_utils.h"

namespace skyfire
{
class server_socket_filter
{
public:
    virtual void send_filter(SOCKET sock, pkg_header_t &header,
                             byte_array &data)
        = 0;
    virtual void recv_filter(SOCKET sock, pkg_header_t &header,
                             byte_array &data)
        = 0;
    virtual void raw_send_filter(SOCKET sock, byte_array &data) = 0;
    virtual void raw_recv_filter(SOCKET sock, byte_array &data) = 0;
    virtual void new_connection_filter(SOCKET sock) = 0;
    virtual void listen_sock_filter(SOCKET sock) = 0;
    virtual void disconnect_sock_filter(SOCKET sock) = 0;
    virtual ~server_socket_filter() = default;
};
} // namespace skyfire