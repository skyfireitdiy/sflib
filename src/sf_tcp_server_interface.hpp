
#pragma once
#include "sf_net_utils.h"
#include "sf_tcp_server_interface.h"
namespace skyfire
{
inline bool tcp_server_interface::server_addr(addr_info_t& addr)
{
    return local_addr(raw_socket(), addr);
}
inline void tcp_server_interface::after_raw_recv_filter__(SOCKET      sock,
                                                          byte_array& data)
{
    for (auto& p : filters__)
    {
        p->raw_recv_filter(sock, data);
    }
}
inline void tcp_server_interface::after_recv_filter__(
    SOCKET sock, pkg_header_t& header, byte_array& data)
{
    for (auto& p : filters__)
    {
        p->recv_filter(sock, header, data);
    }
}
inline void tcp_server_interface::before_raw_send_filter__(
    SOCKET sock, byte_array& data)
{
    for (auto& p : filters__)
    {
        p->raw_send_filter(sock, data);
    }
}
inline void tcp_server_interface::before_send_filter__(
    SOCKET sock, pkg_header_t& header, byte_array& data)
{
    for (auto& p : filters__)
    {
        p->send_filter(sock, header, data);
    }
}
inline void tcp_server_interface::new_connection_filter__(SOCKET sock)
{
    for (auto& p : filters__)
    {
        p->new_connection_filter(sock);
    }
}
inline void tcp_server_interface::listen_sock_filter__(SOCKET sock)
{
    for (auto& p : filters__)
    {
        p->listen_sock_filter(sock);
    }
}
inline void tcp_server_interface::disconnect_sock_filter__(SOCKET sock)
{
    for (auto& p : filters__)
    {
        p->disconnect_sock_filter(sock);
    }
}
inline void tcp_server_interface::add_server_socket_filter(
    std::shared_ptr<server_socket_filter>&& filter)
{ // NOLINT(performance-unnecessary-value-param)
    filters__.emplace_back(std::move(filter));
}
} // namespace skyfire