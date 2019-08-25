
/**
* @version 1.0.0
* @author skyfire
* @mail skyfireitdiy@hotmail.com
* @see http://github.com/skyfireitdiy/sflib
* @file sf_tcp_client_win.hpp

* sflib第一版本发布
* 版本号1.0.0
* 发布日期：2018-10-22
*/

#pragma once

#include "sf_tcp_client_interface.hpp"
#include "sf_tcp_client_win.h"
#include "sf_tcp_utils.hpp"

namespace skyfire {
inline SOCKET sf_tcp_client::raw_socket() { return sock__; }

inline bool sf_tcp_client::bind(const std::string &ip, unsigned short port) {
    sockaddr_in address;
    address.sin_family = AF_INET;
    // ReSharper disable once CppDeprecatedEntity
    address.sin_addr.S_un.S_addr = inet_addr(ip.c_str());
    address.sin_port = htons(port);
    return SOCKET_ERROR != ::bind(sock__,
                                  reinterpret_cast<sockaddr *>(&address),
                                  sizeof(address));
}

inline sf_tcp_client::sf_tcp_client(SOCKET sock, bool raw) {
    sock__ = sock;
    inited__ = true;
    raw__ = raw;
    std::thread(&sf_tcp_client::recv_thread__, this).detach();
}

inline sf_tcp_client::sf_tcp_client(bool raw) {
    WSADATA wsa_data{};
    if (WSAStartup(MAKEWORD(2, 2), &wsa_data) != 0) {
        inited__ = false;
        return;
    }

    sock__ = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock__ == INVALID_SOCKET) {
        inited__ = false;
        return;
    }

    int op = 1;
    if (SOCKET_ERROR == setsockopt(sock__, SOL_SOCKET, SO_REUSEADDR,
                                   reinterpret_cast<char *>(&op), sizeof(op))) {
        inited__ = false;
        return;
    }

    inited__ = true;
    raw__ = raw;
}

inline sf_tcp_client::~sf_tcp_client() {
    close();
    WSACleanup();
}

inline bool sf_tcp_client::connect_to_server(const std::string &ip,
                                             unsigned short port) {
    if (!inited__) return false;
    sockaddr_in address;
    address.sin_family = AF_INET;
    // ReSharper disable once CppDeprecatedEntity
    address.sin_addr.S_un.S_addr = inet_addr(ip.c_str());
    address.sin_port = htons(port);
    if (::connect(sock__, reinterpret_cast<const sockaddr *>(&address),
                  sizeof(address)) != 0) {
        return false;
    }
    std::thread(&sf_tcp_client::recv_thread__, this).detach();
    return true;
}

void sf_tcp_client::recv_thread__() {
    byte_array recv_buffer(sf_default_buffer_size);
    byte_array data;
    sf_pkg_header_t header;
    while (true) {
        const auto len =
            ::recv(sock__, recv_buffer.data(), sf_default_buffer_size, 0);
        if (len <= 0) {
            closed();
            break;
        }
        if (raw__) {
            raw_data_coming(
                byte_array(recv_buffer.begin(), recv_buffer.begin() + len));
        } else {
            data.insert(data.end(), recv_buffer.begin(),
                        recv_buffer.begin() + len);
            size_t read_pos = 0;
            while (data.size() - read_pos >= sizeof(sf_pkg_header_t)) {
                memmove_s(&header, sizeof(header), data.data() + read_pos,
                          sizeof(header));
                if (!check_header_checksum(header)) {
                    close();
                    return;
                }
                if (data.size() - read_pos - sizeof(header) >= header.length) {
                    data_coming(
                        header,
                        byte_array(data.begin() + read_pos + sizeof(header),
                                   data.begin() + read_pos + sizeof(header) +
                                       header.length));
                    read_pos += sizeof(header) + header.length;
                } else {
                    break;
                }
            }
            if (read_pos != 0) {
                data.erase(data.begin(), data.begin() + read_pos);
            }
        }
    }
}

inline bool sf_tcp_client::send(int type, const byte_array &data) {
    if (!inited__) return false;
    sf_pkg_header_t header;
    header.type = type;
    header.length = data.size();
    make_header_checksum(header);
    const auto ret = ::send(sock__, make_pkg(header).data(), sizeof(header), 0);
    if (ret != sizeof(header)) return false;
    return ::send(sock__, data.data(), data.size(), 0) == data.size();
}

inline bool sf_tcp_client::send(const byte_array &data) {
    if (!inited__) return false;
    return ::send(sock__, data.data(), data.size(), 0) == data.size();
}

inline void sf_tcp_client::close() {
    if (!inited__) return;
    shutdown(sock__, SD_BOTH);
    closesocket(sock__);
    sock__ = INVALID_SOCKET;
}

}    // namespace skyfire
