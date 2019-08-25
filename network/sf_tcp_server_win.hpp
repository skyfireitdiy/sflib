
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

#include "network/sf_net_utils.hpp"
#include "sf_tcp_server_interface.hpp"
#include "sf_tcp_server_win.h"
#include "tools/sf_logger.hpp"

namespace skyfire {

inline SOCKET sf_tcp_server::raw_socket() { return listen_sock__; }

void sf_tcp_server::handle_sock_error__(sf_per_handle_data_t *p_handle_data,
                                        sf_per_io_operation_data_t *p_io_data,
                                        int index) {
    disconnect_sock_filter__(p_handle_data->socket);
    close(p_handle_data->socket);
    closed(p_handle_data->socket);
    delete p_handle_data;
    delete p_io_data;
}

void sf_tcp_server::write_handle__(DWORD bytes_transferred,
                                   sf_per_handle_data_t *p_handle_data,
                                   sf_per_io_operation_data_t *p_io_data,
                                   int index) {
    auto invalid = false;
    {
        p_io_data->data_trans_count += bytes_transferred;

        // 没发完
        if (p_io_data->data_trans_count != p_io_data->buffer.size()) {
            sf_debug("not send complete", p_io_data->data_trans_count,
                     p_io_data->buffer.size());
            ZeroMemory(&p_io_data->overlapped, sizeof(p_io_data->overlapped));
            p_io_data->wsa_buffer.buf =
                p_io_data->buffer.data() + p_io_data->data_trans_count;
            p_io_data->wsa_buffer.len =
                p_io_data->buffer.size() - p_io_data->data_trans_count;
            DWORD tmp_int = 0;
            if (WSASend(p_handle_data->socket, &(p_io_data->wsa_buffer), 1,
                        &tmp_int, 0, &(p_io_data->overlapped),
                        nullptr) == SOCKET_ERROR) {
                const auto err = WSAGetLastError();
                if (err != ERROR_IO_PENDING) {
                    write_error(p_handle_data->socket);
                    invalid = true;
                    sf_debug("write error", err);
                }
            }
        } else {
            // 释放资源，否则会有内存泄露
            write_finished(p_handle_data->socket);
            delete p_io_data;
            p_io_data = nullptr;
        }
    }
    if (invalid) {
        handle_sock_error__(p_handle_data, p_io_data, index);
    }
}

inline void sf_tcp_server::receive_handle__(
    DWORD bytes_transferred, sf_per_handle_data_t *p_handle_data,
    sf_per_io_operation_data_t *p_io_data, int index) {
    auto invalid = false;
    do {
        // 接收
        p_io_data->data_trans_count = bytes_transferred;
        p_io_data->buffer.resize(bytes_transferred);
        auto tmp_data = p_io_data->buffer;
        after_raw_recv_filter__(p_handle_data->socket, tmp_data);
        if (raw__) {
            sf_debug("recv raw", p_io_data->buffer.size());
            raw_data_coming(p_handle_data->socket, tmp_data);
        } else {
            sf_pkg_header_t header{};
            p_handle_data->sock_data_buffer.insert(
                p_handle_data->sock_data_buffer.end(), tmp_data.begin(),
                tmp_data.end());
            size_t read_pos = 0;
            // 循环解析
            while (p_handle_data->sock_data_buffer.size() - read_pos >=
                   sizeof(sf_pkg_header_t)) {
                memmove_s(&header, sizeof(header),
                          p_handle_data->sock_data_buffer.data() + read_pos,
                          sizeof(header));
                if (!check_header_checksum(header)) {
                    invalid = true;
                    break;
                }
                if (p_handle_data->sock_data_buffer.size() - read_pos -
                        sizeof(header) >=
                    header.length) {
                    auto data = byte_array(
                        p_handle_data->sock_data_buffer.begin() + read_pos +
                            sizeof(header),
                        p_handle_data->sock_data_buffer.begin() + read_pos +
                            sizeof(header) + header.length);
                    after_recv_filter__(p_handle_data->socket, header, data);
                    // std::unique_lock<std::mutex>
                    // lck(*p_handle_data->read_lock);
                    data_coming(p_handle_data->socket, header, data);
                    read_pos += sizeof(header) + header.length;
                } else {
                    break;
                }
            }
            if (read_pos != 0) {
                p_handle_data->sock_data_buffer.erase(
                    p_handle_data->sock_data_buffer.begin(),
                    p_handle_data->sock_data_buffer.begin() + read_pos);
            }
        }

        ZeroMemory(&(p_io_data->overlapped), sizeof(p_io_data->overlapped));
        p_io_data->buffer.resize(sf_default_buffer_size);
        p_io_data->data_trans_count = 0;
        p_io_data->is_send = false;
        p_io_data->wsa_buffer.buf = p_io_data->buffer.data();
        p_io_data->wsa_buffer.len = sf_default_buffer_size;

        DWORD flags = 0;
        DWORD tmp_int = 0;
        // 重新发一个读取请求
        if (WSARecv(p_handle_data->socket, &(p_io_data->wsa_buffer), 1,
                    &tmp_int, &flags, &(p_io_data->overlapped),
                    nullptr) == SOCKET_ERROR) {
            if (WSAGetLastError() != ERROR_IO_PENDING) {
                invalid = true;
            }
        }
    } while (false);
    if (invalid) {
        handle_sock_error__(p_handle_data, p_io_data, index);
    }
}

inline void sf_tcp_server::server_work_thread__(int index) {
    iocp_context__[index].iocp_port =
        CreateIoCompletionPort(INVALID_HANDLE_VALUE, nullptr, 0, 0);
    const auto completion_port = iocp_context__[index].iocp_port;
    DWORD bytes_transferred;
    sf_per_handle_data_t *p_handle_data = nullptr;
    sf_per_io_operation_data_t *p_io_data = nullptr;

    while (true) {
        const auto result = GetQueuedCompletionStatus(
            completion_port, &bytes_transferred,
            reinterpret_cast<PULONG_PTR>(&p_handle_data),
            reinterpret_cast<LPOVERLAPPED *>(&p_io_data), WSA_INFINITE);
        auto invalid = false;
        do {
            // 退出标记
            if (exit_flag__) {
                if (p_handle_data != nullptr) {
                    invalid = true;
                }
                sf_debug("exit flag is true");
                return;
            }

            // result为0，请求出错
            if (result == 0) {
                if (p_handle_data != nullptr) {
                    invalid = true;
                }
                sf_debug("result == 0");
                break;
            }

            // 出错
            if (p_handle_data == nullptr || p_io_data == nullptr) {
                invalid = true;
                sf_debug("p_handle_data == nullptr || p_io_data == nullptr");
                break;
            }
            // 关闭
            if (bytes_transferred == 0) {
                invalid = true;
                sf_debug("bytes_transferred == 0");
                break;
            }
        } while (false);

        if (invalid) {
            sf_debug("invalid");
            handle_sock_error__(p_handle_data, p_io_data, index);
            continue;
        }
        // 发送
        if (p_io_data->is_send) {
            write_handle__(bytes_transferred, p_handle_data, p_io_data, index);
        } else {
            receive_handle__(bytes_transferred, p_handle_data, p_io_data,
                             index);
        }
    }
}

bool sf_tcp_server::add_sock__(SOCKET accept_socket) {
    auto index = 0;
    auto min_size = 0;
    {
        std::shared_lock<std::shared_mutex> lck(*iocp_context__[0].mu_socks);
        min_size = iocp_context__[0].socks.size();
    }
    for (auto i = 1; i < thread_count__; ++i) {
        auto sz = 0;
        {
            std::shared_lock<std::shared_mutex> lck(
                *iocp_context__[i].mu_socks);
            sz = iocp_context__[i].socks.size();
        }
        sf_debug("workload", i, sz);
        if (sz < min_size) {
            index = i;
            min_size = sz;
        }
    }

    sf_debug("add socket", accept_socket, index);
    {
        std::unique_lock<std::shared_mutex> lck(
            *iocp_context__[index].mu_socks);
        iocp_context__[index].socks.insert(accept_socket);
    }

    auto p_handle_data = new sf_per_handle_data_t();
    p_handle_data->socket = accept_socket;

    if (CreateIoCompletionPort(reinterpret_cast<HANDLE>(accept_socket),
                               iocp_context__[index].iocp_port,
                               reinterpret_cast<ULONG_PTR>(p_handle_data),
                               0) == nullptr) {
        sf_debug("CreateIoCompletionPort error");
        return false;
    }

    // 生成新的请求数据

    auto p_io_data = make_req__();

    // 填充请求数据
    ZeroMemory(&(p_io_data->overlapped), sizeof(p_io_data->overlapped));
    p_io_data->data_trans_count = 0;
    p_io_data->buffer.resize(sf_default_buffer_size);
    p_io_data->is_send = false;
    p_io_data->wsa_buffer.buf = p_io_data->buffer.data();
    p_io_data->wsa_buffer.len = sf_default_buffer_size;
    DWORD tmp_int = 0;
    DWORD flags = 0;

    // 投递一个接收请求
    sf_debug("post a read request");
    if (WSARecv(accept_socket, &(p_io_data->wsa_buffer), 1, &tmp_int, &flags,
                &(p_io_data->overlapped), nullptr) == SOCKET_ERROR) {
        if (WSAGetLastError() != ERROR_IO_PENDING) {
            handle_sock_error__(p_handle_data, p_io_data, index);
        }
    }
    return true;
}

inline void sf_tcp_server::accept_thread__() {
    while (true) {
        SOCKET accept_socket;
        accept_socket = WSAAccept(listen_sock__, nullptr, nullptr, nullptr, 0);
        if (accept_socket == INVALID_SOCKET) {
            sf_debug("accept_socket == INVALID_SOCKET");
            break;
        }

        sf_debug("new connection");

        // 成功等待到新连接
        new_connection_filter__(accept_socket);
        new_connection(accept_socket);

        if (manage_clients__) {
            if (!add_sock__(accept_socket)) {
                sf_debug("add sock error");
            }
        }
    }
}

inline sf_tcp_server::sf_tcp_server(const bool raw) {
    // 初始化WinSock，线程数量等
    WSADATA wsa_data{};

    if (WSAStartup(MAKEWORD(2, 2), &wsa_data) != 0) {
        return;
    }

    iocp_context__.resize(thread_count__);
    inited__ = true;
    raw__ = raw;
}

inline bool sf_tcp_server::listen(const std::string &ip, unsigned short port) {
    if (!inited__) {
        return false;
    }

    exit_flag__ = false;

    // 创建监听套接字
    listen_sock__ =
        WSASocketW(AF_INET, SOCK_STREAM, 0, nullptr, 0, WSA_FLAG_OVERLAPPED);
    if (listen_sock__ == INVALID_SOCKET) {
        close();
        return false;
    }

    listen_sock_filter__(listen_sock__);

    // 设置地址复用
    int op = 1;
    if (SOCKET_ERROR == setsockopt(listen_sock__, SOL_SOCKET, SO_REUSEADDR,
                                   reinterpret_cast<char *>(&op), sizeof(op))) {
        return false;
    }

    // 创建地址信息
    sockaddr_in internet_addr{};
    internet_addr.sin_family = AF_INET;
    sf_safe_inet_addr(AF_INET, ip.c_str(), internet_addr.sin_addr.s_addr);
    internet_addr.sin_port = htons(port);

    // 绑定端口
    if (SOCKET_ERROR == ::bind(listen_sock__,
                               reinterpret_cast<sockaddr *>(&internet_addr),
                               sizeof(internet_addr))) {
        close();
        return false;
    }

    // 监听
    if (::listen(listen_sock__, max_tcp_connection) == SOCKET_ERROR) {
        close();
        return false;
    }

    // 启动工作线程
    for (auto i = 0; i < thread_count__; ++i) {
        sf_debug("start work thread");
        thread_vec__.emplace_back(
            std::thread(&sf_tcp_server::server_work_thread__, this, i));
    }

    // 启动线程accept
    thread_vec__.emplace_back(
        std::thread(&sf_tcp_server::accept_thread__, this));
    return true;
}

inline sf_per_io_operation_data_t *sf_tcp_server::make_req__() {
    return new sf_per_io_operation_data_t();
}

inline void sf_tcp_server::close() {
    exit_flag__ = true;

    for (auto i = 0; i < thread_count__; ++i) {
        std::unique_lock<std::shared_mutex> lck(*iocp_context__[i].mu_socks);
        for (auto &p : iocp_context__[i].socks) {
            ::shutdown(p, SD_BOTH);
            ::closesocket(p);
        }
        iocp_context__[i].socks.clear();
        PostQueuedCompletionStatus(iocp_context__[i].iocp_port, 0, 0, nullptr);
        CloseHandle(iocp_context__[i].iocp_port);
    }

    if (listen_sock__ != INVALID_SOCKET) {
        shutdown(listen_sock__, SD_BOTH);
        closesocket(listen_sock__);
        listen_sock__ = INVALID_SOCKET;
    }
    for (auto &p : thread_vec__) {
        p.join();
    }
}

inline void sf_tcp_server::close(SOCKET sock) {
    ::shutdown(sock, SD_BOTH);
    ::closesocket(sock);
    for (auto &p : iocp_context__) {
        std::unique_lock<std::shared_mutex> lck(*p.mu_socks);
        if (p.socks.count(sock) != 0) {
            p.socks.erase(sock);
            break;
        }
    }
}

inline bool sf_tcp_server::detach(SOCKET sock) { return false; }

inline bool sf_tcp_server::send(SOCKET sock, int type, const byte_array &data) {
    DWORD send_bytes;
    sf_pkg_header_t header{};
    header.type = type;
    header.length = data.size();
    make_header_checksum(header);
    auto tmp_data = data;
    before_send_filter__(sock, header, tmp_data);
    auto send_data = make_pkg(header) + tmp_data;
    before_raw_send_filter__(sock, send_data);
    auto p_io_data = make_req__();
    ZeroMemory(&(p_io_data->overlapped), sizeof(p_io_data->overlapped));
    p_io_data->buffer = send_data;
    p_io_data->data_trans_count = 0;
    p_io_data->is_send = true;
    p_io_data->wsa_buffer.buf = p_io_data->buffer.data();
    p_io_data->wsa_buffer.len = p_io_data->buffer.size();
    if (WSASend(sock, &(p_io_data->wsa_buffer), 1, &send_bytes, 0,
                &(p_io_data->overlapped), nullptr) == SOCKET_ERROR) {
        if (WSAGetLastError() != ERROR_IO_PENDING) {
            delete p_io_data;
            p_io_data = nullptr;
            return false;
        }
    }

    return true;
}

inline bool sf_tcp_server::send(SOCKET sock, const byte_array &data) {
    DWORD send_bytes;

    auto tmp_data = data;
    before_raw_send_filter__(sock, tmp_data);

    auto p_io_data = make_req__();
    ZeroMemory(&(p_io_data->overlapped), sizeof(p_io_data->overlapped));
    p_io_data->buffer = tmp_data;
    p_io_data->data_trans_count = 0;
    p_io_data->is_send = true;
    p_io_data->wsa_buffer.buf = p_io_data->buffer.data();
    p_io_data->wsa_buffer.len = p_io_data->buffer.size();
    if (WSASend(sock, &(p_io_data->wsa_buffer), 1, &send_bytes, 0,
                &(p_io_data->overlapped), nullptr) == SOCKET_ERROR) {
        if (WSAGetLastError() != ERROR_IO_PENDING) {
            delete p_io_data;
            p_io_data = nullptr;
            return false;
        }
    }
    return true;
}

inline sf_tcp_server::~sf_tcp_server() {
    sf_tcp_server::close();
    WSACleanup();
}

}    // namespace skyfire