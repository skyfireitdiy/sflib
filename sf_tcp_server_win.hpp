
/**
* @version 1.0.0
* @author skyfire
* @mail skyfireitdiy@hotmail.com
* @see http://github.com/skyfireitdiy/sflib
* @file sf_tcp_server_win.hpp

* sflib第一版本发布
* 版本号1.0.0
* 发布日期：2018-10-22
*/

#pragma once

#include "sf_tcp_server_win.h"
#include "sf_tcp_client_interface.hpp"
#include "sf_logger.hpp"

namespace skyfire {

    inline SOCKET sf_tcp_server::get_raw_socket() {
        return listen_sock__;
    }

    inline void sf_tcp_server::server_work_thread__(LPVOID completion_port_id) {
        auto completion_port = (HANDLE) completion_port_id;
        DWORD bytesTransferred;
        sf_per_handle_data_t *p_handle_data = nullptr;
        sf_per_io_operation_data_t *p_io_data = nullptr;

        DWORD tmp_int = 0;
        DWORD flags = 0;

        while (true) {
            int result = GetQueuedCompletionStatus(completion_port, &bytesTransferred, (PULONG_PTR) &p_handle_data,
                                                   (LPOVERLAPPED *) &p_io_data, WSA_INFINITE);
            if (exit_flag__) {
                if (p_handle_data != nullptr) {
                    closed(p_handle_data->socket);
                    delete p_handle_data;
                    delete p_io_data;
                    sock_data_buffer__.erase(p_handle_data->socket);
                }
                break;
            }

            if (result == 0) {
                if (p_handle_data != nullptr) {
                    closed(p_handle_data->socket);
                    delete p_handle_data;
                    delete p_io_data;
                    sock_data_buffer__.erase(p_handle_data->socket);
                }

                continue;
            }

            if (p_handle_data == nullptr || p_io_data == nullptr) {
                if(p_handle_data)
                {
                    closed(p_handle_data->socket);
                }
                delete p_handle_data;
                delete p_io_data;
                continue;
            }
            if (bytesTransferred == 0) {
                closed(p_handle_data->socket);
                delete p_handle_data;
                delete p_io_data;
                continue;
            }
            if (p_io_data->is_send) {
                p_io_data->data_trans_count += bytesTransferred;

                if (p_io_data->data_trans_count != p_io_data->buffer.size()) {
                    ZeroMemory(&p_io_data->overlapped, sizeof(p_io_data->overlapped));
                    p_io_data->wsa_buffer.buf = p_io_data->buffer.data() + p_io_data->data_trans_count;
                    p_io_data->wsa_buffer.len = p_io_data->buffer.size() - p_io_data->data_trans_count;
                    tmp_int = 0;
                    if (WSASend(p_handle_data->socket, &(p_io_data->wsa_buffer), 1, &tmp_int, 0,
                                &(p_io_data->overlapped),
                                nullptr) == SOCKET_ERROR) {
                        if (WSAGetLastError() != ERROR_IO_PENDING) {
                            closed(p_handle_data->socket);
                            delete p_handle_data;
                            delete p_io_data;
                            continue;
                        }
                    }
                }
                else
                {
                    // 释放资源，否则会有内存泄露
                    delete p_io_data;
                }
            } else {
                p_io_data->data_trans_count = bytesTransferred;
                p_io_data->buffer.resize(bytesTransferred);

                if (raw__) {
                    sf_debug("recv raw",p_io_data->buffer.size());
                    raw_data_coming(p_handle_data->socket, p_io_data->buffer);
                } else {
                    sf_pkg_header_t header{};
                    sock_data_buffer__[p_handle_data->socket].insert(
                            sock_data_buffer__[p_handle_data->socket].end(),
                            p_io_data->buffer.begin(),
                            p_io_data->buffer.end());
                    size_t read_pos = 0;
                    while (sock_data_buffer__[p_handle_data->socket].size() - read_pos >= sizeof(sf_pkg_header_t)) {
                        memmove_s(&header, sizeof(header),
                                  sock_data_buffer__[p_handle_data->socket].data() + read_pos,
                                  sizeof(header));
                        if (!check_header_checksum(header)) {
                            close(p_handle_data->socket);
                            break;
                        }
                        if (sock_data_buffer__[p_handle_data->socket].size() - read_pos - sizeof(header) >=
                            header.length) {
                            data_coming(
                                    p_handle_data->socket,
                                    header,
                                    byte_array(
                                            sock_data_buffer__[p_handle_data->socket].begin() + read_pos +
                                            sizeof(header),
                                            sock_data_buffer__[p_handle_data->socket].begin() + read_pos +
                                            sizeof(header) + header.length)
                            );
                            read_pos += sizeof(header) + header.length;
                        } else {
                            break;
                        }
                    }
                    if (read_pos != 0) {
                        sock_data_buffer__[p_handle_data->socket].erase(
                                sock_data_buffer__[p_handle_data->socket].begin(),
                                sock_data_buffer__[p_handle_data->socket].begin() + read_pos);
                    }
                }

                ZeroMemory(&(p_io_data->overlapped), sizeof(p_io_data->overlapped));
                p_io_data->buffer.resize(SF_DEFAULT_BUFFER_SIZE);
                p_io_data->data_trans_count = 0;
                p_io_data->is_send = false;
                p_io_data->wsa_buffer.buf = p_io_data->buffer.data();
                p_io_data->wsa_buffer.len = SF_DEFAULT_BUFFER_SIZE;

                flags = 0;

                if (WSARecv(p_handle_data->socket, &(p_io_data->wsa_buffer), 1, &tmp_int, &flags,
                            &(p_io_data->overlapped), nullptr) == SOCKET_ERROR) {
                    if (WSAGetLastError() != ERROR_IO_PENDING) {
                        closed(p_handle_data->socket);
                        delete p_handle_data;
                        delete p_io_data;
                        continue;
                    }
                }
            }
        }
    }

    inline sf_tcp_server::sf_tcp_server(bool raw) {
        DWORD ret;
        WSADATA wsa_data{};

        if (WSAStartup(MAKEWORD(2, 2), &wsa_data) != 0) {
            return;
        }

        SYSTEM_INFO sys_info{};
        GetSystemInfo(&sys_info);
        thread_count__ = static_cast<int>(sys_info.dwNumberOfProcessors * 2 + 2);
        inited__ = true;
        raw__ = raw;
    }

    inline bool sf_tcp_server::listen(const std::string &ip, unsigned short port) {
        if (!inited__) {
            return false;
        }

        exit_flag__ = false;

        DWORD ret;
        completion_port__ = CreateIoCompletionPort(INVALID_HANDLE_VALUE, nullptr, 0, 0);
        if (completion_port__ == nullptr) {
            close();
            return false;
        }

        for (auto i : sf_range(thread_count__)) {
            std::thread([=] { server_work_thread__(completion_port__); }).detach();
        }
        listen_sock__ = WSASocket(AF_INET, SOCK_STREAM, 0, nullptr, 0, WSA_FLAG_OVERLAPPED);
        if (listen_sock__ == INVALID_SOCKET) {
            close();
            return false;
        }

        int op = 1;
        if(SOCKET_ERROR == setsockopt(listen_sock__,
                                      SOL_SOCKET,
                                      SO_REUSEADDR,
                                      reinterpret_cast<char*>(&op),
                                      sizeof(op))){
            return false;
        }

        sockaddr_in internet_addr{};
        internet_addr.sin_family = AF_INET;
        internet_addr.sin_addr.s_addr = inet_addr(ip.c_str());
        internet_addr.sin_port = htons(port);

        if (SOCKET_ERROR ==
            ::bind(listen_sock__, reinterpret_cast<sockaddr *>(&internet_addr), sizeof(internet_addr))) {
            close();
            return false;
        }

        if (::listen(listen_sock__, SOMAXCONN) == SOCKET_ERROR) {
            close();
            return false;
        }

        std::thread([=]() {
            while (true) {
                SOCKET accept_socket;
                sockaddr_in addr;
                int addr_len = sizeof(addr);
                accept_socket = WSAAccept(listen_sock__, nullptr, nullptr, nullptr, 0);
                if (accept_socket == INVALID_SOCKET) {
                    break;
                }

                new_connection(accept_socket);

                auto *p_handle_data = new sf_per_handle_data_t;
                p_handle_data->socket = accept_socket;
                if (CreateIoCompletionPort((HANDLE) accept_socket, completion_port__,
                                           (ULONG_PTR) p_handle_data, 0) == nullptr) {
                    break;
                }
                auto *p_io_data = new sf_per_io_operation_data_t;
                ZeroMemory(&(p_io_data->overlapped), sizeof(p_io_data->overlapped));
                p_io_data->data_trans_count = 0;
                p_io_data->buffer.resize(SF_DEFAULT_BUFFER_SIZE);
                p_io_data->is_send = false;
                p_io_data->wsa_buffer.buf = p_io_data->buffer.data();
                p_io_data->wsa_buffer.len = SF_DEFAULT_BUFFER_SIZE;
                DWORD tmp_int = 0;
                DWORD flags = 0;
                if (WSARecv(accept_socket, &(p_io_data->wsa_buffer), 1, &tmp_int, &flags, &(p_io_data->overlapped),
                            nullptr) ==
                    SOCKET_ERROR) {
                    if (WSAGetLastError() != ERROR_IO_PENDING) {
                        delete p_handle_data;
                        delete p_io_data;
                        continue;
                    }
                }
            }
        }).detach();
        return true;
    }

    inline std::shared_ptr<sf_tcp_server> sf_tcp_server::make_server(bool raw) {
        return std::make_shared<sf_tcp_server>(raw);
    }

    inline void sf_tcp_server::close() {
        exit_flag__ = true;
        if (completion_port__ != nullptr) {
            for (auto i : sf_range(thread_count__)) {
                PostQueuedCompletionStatus(completion_port__, 0, 0, nullptr);
            }
            CloseHandle(completion_port__);
            completion_port__ = INVALID_HANDLE_VALUE;
        }
        sock_data_buffer__.clear();
        if (listen_sock__ != INVALID_SOCKET) {
            shutdown(listen_sock__,SD_BOTH);
            closesocket(listen_sock__);
            listen_sock__ = INVALID_SOCKET;
        }
    }

    inline void sf_tcp_server::close(SOCKET sock) {
        shutdown(sock,SD_BOTH);
        ::closesocket(sock);
    }

    inline bool sf_tcp_server::send(SOCKET sock, int type, const byte_array &data) {
        DWORD sendBytes;

        sf_pkg_header_t header;
        header.type = type;
        header.length = data.size();
        make_header_checksum(header);
        auto p_io_data = new sf_per_io_operation_data_t;
        ZeroMemory(&(p_io_data->overlapped), sizeof(p_io_data->overlapped));
        p_io_data->buffer = make_pkg(header) + data;
        p_io_data->data_trans_count = 0;
        p_io_data->is_send = true;
        p_io_data->wsa_buffer.buf = p_io_data->buffer.data();
        p_io_data->wsa_buffer.len = p_io_data->buffer.size();
        if (WSASend(sock, &(p_io_data->wsa_buffer), 1, &sendBytes, 0, &(p_io_data->overlapped),
                    nullptr) == SOCKET_ERROR) {
            if (WSAGetLastError() != ERROR_IO_PENDING) {
                delete p_io_data;
                return false;
            }
        }
        return true;
    }

    inline bool sf_tcp_server::send(SOCKET sock, const byte_array &data) {
        DWORD sendBytes;

        auto p_io_data = new sf_per_io_operation_data_t;
        ZeroMemory(&(p_io_data->overlapped), sizeof(p_io_data->overlapped));
        p_io_data->buffer = data;
        p_io_data->data_trans_count = 0;
        p_io_data->is_send = true;
        p_io_data->wsa_buffer.buf = p_io_data->buffer.data();
        p_io_data->wsa_buffer.len = p_io_data->buffer.size();
        if (WSASend(sock, &(p_io_data->wsa_buffer), 1, &sendBytes, 0, &(p_io_data->overlapped),
                    nullptr) == SOCKET_ERROR) {
            if (WSAGetLastError() != ERROR_IO_PENDING) {
                delete p_io_data;
                return false;
            }
        }
        return true;
    }

    inline sf_tcp_server::~sf_tcp_server() {
        close();
        WSACleanup();
    }


}