
/**
* @version 1.0.0
* @author skyfire
* @mail skyfireitdiy@hotmail.com
* @see http://github.com/skyfireitdiy/sflib
* @file sf_tcp_server_interface.hpp

* sflib第一版本发布
* 版本号1.0.0
* 发布日期：2018-10-22
*/

#pragma once

#include "sf_tcp_server_interface.h"

namespace skyfire {
inline bool sf_tcp_server_interface::server_addr(sf_addr_info_t &addr) {
    return local_addr(raw_socket(), addr);
}

inline void sf_tcp_server_interface::after_raw_recv_filter__(SOCKET sock,
                                                             byte_array &data) {
    for (auto &p : filters__) {
        p->raw_recv_filter(sock, data);
    }
}

inline void sf_tcp_server_interface::after_recv_filter__(
    SOCKET sock, sf_pkg_header_t &header, byte_array &data) {
    for (auto &p : filters__) {
        p->recv_filter(sock, header, data);
    }
}

inline void sf_tcp_server_interface::before_raw_send_filter__(
    SOCKET sock, byte_array &data) {
    for (auto &p : filters__) {
        p->raw_send_filter(sock, data);
    }
}

inline void sf_tcp_server_interface::before_send_filter__(
    SOCKET sock, sf_pkg_header_t &header, byte_array &data) {
    for (auto &p : filters__) {
        p->send_filter(sock, header, data);
    }
}

inline void sf_tcp_server_interface::new_connection_filter__(SOCKET sock) {
    for (auto &p : filters__) {
        p->new_connection_filter(sock);
    }
}

inline void sf_tcp_server_interface::listen_sock_filter__(SOCKET sock) {
    for (auto &p : filters__) {
        p->listen_sock_filter(sock);
    }
}

inline void sf_tcp_server_interface::disconnect_sock_filter__(SOCKET sock) {
    for (auto &p : filters__) {
        p->disconnect_sock_filter(sock);
    }
}

#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCUnusedGlobalDeclarationInspection"

inline void sf_tcp_server_interface::add_server_socket_filter(
    std::shared_ptr<sf_server_socket_filter>
        &&filter) {    // NOLINT(performance-unnecessary-value-param)
    filters__.emplace_back(std::move(filter));
}

inline void sf_tcp_server_interface::set_manage_clients(bool flag) {
    manage_clients__ = flag;
}
#pragma clang diagnostic pop
}    // namespace skyfire