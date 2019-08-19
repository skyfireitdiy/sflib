
/**
* @version 1.0.0
* @author skyfire
* @mail skyfireitdiy@hotmail.com
* @see http://github.com/skyfireitdiy/sflib
* @file sf_tcp_server_linux.hpp

* sflib第一版本发布
* 版本号1.0.0
* 发布日期：2018-10-22
*/

#pragma once

#include "core/sf_define.h"
#include "network/sf_net_utils.hpp"
#include "sf_tcp_server_interface.hpp"
#include "sf_tcp_server_linux.h"

namespace skyfire {
inline sf_tcp_server::sf_tcp_server(const bool raw) { raw__ = raw; }

inline SOCKET sf_tcp_server::raw_socket() { return listen_fd__; }

inline bool sf_tcp_server::send(int sock, const byte_array &data) {
    auto &sock_context__ = epoll_data__().sock_context__;
    auto send_data = data;
    before_raw_send_filter__(sock, send_data);

    {
        std::lock_guard<std::shared_mutex> lck(
            *sock_context__[sock].mu_data_buffer_out);
        sock_context__[sock].data_buffer_out.push_back(send_data);
    }

    sock_context__[sock].ev.events |= EPOLLOUT;

    return epoll_ctl(epoll_data__().epoll_fd, EPOLL_CTL_MOD, sock,
                     &sock_context__[sock].ev) != -1;
}

inline bool sf_tcp_server::send(int sock, int type, const byte_array &data) {
    auto &sock_context__ = epoll_data__().sock_context__;

    sf_pkg_header_t header{};
    header.type = type;
    header.length = data.size();
    make_header_checksum(header);
    auto tmp_data = data;
    before_send_filter__(sock, header, tmp_data);
    auto send_data = make_pkg(header) + tmp_data;

    before_raw_send_filter__(sock, send_data);

    {
        std::shared_lock<std::shared_mutex> lck(
            *sock_context__[sock].mu_data_buffer_out);
        sock_context__[sock].data_buffer_out.push_back(send_data);
    }

    sock_context__[sock].ev.events |= EPOLLOUT;

    return epoll_ctl(epoll_data__().epoll_fd, EPOLL_CTL_MOD, sock,
                     &sock_context__[sock].ev) != -1;
}

inline void sf_tcp_server::close(SOCKET sock) {
    ::shutdown(sock, SHUT_RDWR);
    ::close(sock);
}

inline void sf_tcp_server::close() {
    shutdown(listen_fd__, SHUT_RDWR);
    ::close(listen_fd__);
    listen_fd__ = -1;

    for (auto &p : thread_vec__) {
        p.join();
    }
}

inline bool sf_tcp_server::listen(const std::string &ip, unsigned short port) {
    listen_fd__ = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_fd__ == -1) {
        return false;
    }

    listen_sock_filter__(listen_fd__);

    if (fcntl(listen_fd__, F_SETFL,
              fcntl(listen_fd__, F_GETFD, 0) | O_NONBLOCK) == -1) {
        return false;
    }

    int opt = 1;
    if (-1 == setsockopt(listen_fd__, SOL_SOCKET, SO_REUSEADDR,
                         reinterpret_cast<const void *>(&opt), sizeof(opt))) {
        return false;
    }

    if (-1 == setsockopt(listen_fd__, SOL_SOCKET, SO_REUSEPORT,
                         reinterpret_cast<const void *>(&opt), sizeof(opt))) {
        return false;
    }

    sockaddr_in internet_addr{};
    internet_addr.sin_family = AF_INET;
    internet_addr.sin_addr.s_addr = inet_addr(ip.c_str());
    internet_addr.sin_port = htons(port);

    if (::bind(listen_fd__, reinterpret_cast<sockaddr *>(&internet_addr),
               sizeof(sockaddr_in)) == -1) {
        return false;
    }

    if (::listen(listen_fd__, max_tcp_connection) == -1) {
        return false;
    }

    thread_vec__.emplace_back(
        std::thread(&sf_tcp_server::work_thread__, this, true, listen_fd__));

    if (manage_clients__) {
        for (int i = 1; i < thread_count__; ++i) {
            thread_vec__.emplace_back(std::thread(&sf_tcp_server::work_thread__,
                                                  this, true, listen_fd__));
        }
    }
    return true;
}

inline void sf_tcp_server::work_thread__(bool listen_thread, SOCKET listen_fd) {
    sf_debug("start thread");
    epoll_data__().epoll_fd = epoll_create(max_tcp_connection);

    auto &sock_context__ = epoll_data__().sock_context__;

    if (listen_thread) {
        sock_context__[listen_fd] = sock_data_context_t{};
        sock_context__[listen_fd].ev.events = EPOLLIN | EPOLLET;
        sock_context__[listen_fd].ev.data.fd = listen_fd;

        if (epoll_ctl(epoll_data__().epoll_fd, EPOLL_CTL_ADD, listen_fd,
                      &sock_context__[listen_fd].ev) < 0) {
            sf_debug("add to epoll error");
            close(listen_fd);
            return;
        }
    }
    std::vector<epoll_event> evs(max_tcp_connection);

    while (true) {
        int wait_fds = 0;
        if ((wait_fds = epoll_wait(epoll_data__().epoll_fd, evs.data(),
                                   max_tcp_connection, -1)) == -1) {
            if (errno == EINTR) {
                continue;
            }
            break;
        }

        if (wait_fds == 0) {
            continue;
        }

        sf_debug("new epoll event", wait_fds);

        auto listen_err = false;

        for (auto i = 0; i < wait_fds; ++i) {
            if (evs[i].data.fd == listen_fd__ && (evs[i].events & EPOLLIN)) {
                if (!handle_accept__()) {
                    listen_err = true;
                }
            } else if (evs[i].events & EPOLLIN) {
                handle_read__(evs[i]);
            } else if (evs[i].events & EPOLLOUT) {
                handle_write__(evs[i]);
            }
        }
        if (listen_err) {
            sf_debug("listen error");
            break;
        }
    }
}

inline sf_tcp_server::~sf_tcp_server() { close(); }

inline bool sf_tcp_server::in_dispatch__(SOCKET fd) {
    sf_debug("add new fd", fd);

    auto &sock_context__ = epoll_data__().sock_context__;
    sock_context__[fd] = sock_data_context_t{};
    sock_context__[fd].ev.events = EPOLLIN | EPOLLET;
    sock_context__[fd].ev.data.fd = fd;

    if (epoll_ctl(epoll_data__().epoll_fd, EPOLL_CTL_ADD, fd,
                  &sock_context__[fd].ev) < 0) {
        sf_debug("add to epoll error");
        close(fd);
        return false;
    }
    return true;
}

inline bool sf_tcp_server::handle_accept__() {
    int conn_fd = 0;
    sockaddr_in client_addr{};
    socklen_t len = sizeof(client_addr);
    while ((conn_fd = accept(listen_fd__, (struct sockaddr *)&client_addr,
                             &len)) > 0) {
        new_connection_filter__(conn_fd);
        if (manage_clients__) {
            if (fcntl(conn_fd, F_SETFL,
                      fcntl(conn_fd, F_GETFD, 0) | O_NONBLOCK) == -1) {
                sf_debug("set no block error");
                close(conn_fd);
                return true;
            }
            if (!in_dispatch__(conn_fd)) {
                return true;
            }
        }
        sf_debug("new connection", conn_fd);
        new_connection(conn_fd);
    }

    if (errno == EAGAIN || errno == EINTR) {
        return true;
    } else {
        sf_debug("accept error");
        {
            epoll_ctl(epoll_data__().epoll_fd, EPOLL_CTL_DEL, listen_fd__,
                      &epoll_data__().sock_context__[listen_fd__].ev);
            epoll_data__().sock_context__.erase(listen_fd__);
        }
        return false;
    }
}

inline void sf_tcp_server::handle_read__(const epoll_event &ev) {
    byte_array recv_buf(sf_default_buffer_size);
    sf_pkg_header_t header{};
    auto &sock_context__ = epoll_data__().sock_context__;
    while (true) {
        sf_debug("start read");
        auto count_read = static_cast<int>(
            recv(ev.data.fd, recv_buf.data(), sf_default_buffer_size, 0));
        sf_debug("read", count_read);
        if (count_read <= 0) {
            sf_debug("errno", errno);
            // EWOULDBLOCK == EAGAIN
            if ((errno == EAGAIN ||
                 errno == EINTR /* || errno == EWOULDBLOCK */) &&
                count_read < 0) {
                sf_debug("read finished", errno);
                break;
            } else {
                disconnect_sock_filter__(ev.data.fd);
                {
                    epoll_ctl(epoll_data__().epoll_fd, EPOLL_CTL_DEL,
                              ev.data.fd, &sock_context__[ev.data.fd].ev);
                    sock_context__.erase(ev.data.fd);
                }
                closed(ev.data.fd);
                close(ev.data.fd);
                sf_debug("read error / connection closed");
                break;
            }
        }
        recv_buf.resize(static_cast<unsigned long>(count_read));
        after_raw_recv_filter__(ev.data.fd, recv_buf);
        if (raw__) {
            sf_debug("raw data", recv_buf.size());
            raw_data_coming(ev.data.fd, recv_buf);
            sf_debug("after resolve");
        } else {
            sock_context__[ev.data.fd].data_buffer_in.insert(
                sock_context__[ev.data.fd].data_buffer_in.end(),
                recv_buf.begin(), recv_buf.end());
            size_t read_pos = 0;
            while (sock_context__[ev.data.fd].data_buffer_in.size() -
                       read_pos >=
                   sizeof(sf_pkg_header_t)) {
                memmove(
                    &header,
                    sock_context__[ev.data.fd].data_buffer_in.data() + read_pos,
                    sizeof(header));
                if (!check_header_checksum(header)) {
                    disconnect_sock_filter__(ev.data.fd);
                    close(ev.data.fd);
                    closed(ev.data.fd);
                    break;
                }
                if (sock_context__[ev.data.fd].data_buffer_in.size() -
                        read_pos - sizeof(header) >=
                    header.length) {
                    byte_array data = {byte_array(
                        sock_context__[ev.data.fd].data_buffer_in.begin() +
                            read_pos + sizeof(header),
                        sock_context__[ev.data.fd].data_buffer_in.begin() +
                            read_pos + sizeof(header) + header.length)};
                    read_pos += sizeof(header) + header.length;

                    after_recv_filter__(ev.data.fd, header, data);
                    data_coming(ev.data.fd, header, data);

                } else {
                    break;
                }
            }
            if (read_pos != 0) {
                sock_context__[ev.data.fd].data_buffer_in.erase(
                    sock_context__[ev.data.fd].data_buffer_in.begin(),
                    sock_context__[ev.data.fd].data_buffer_in.begin() +
                        read_pos);
            }
        }
    }
}

inline void sf_tcp_server::handle_write__(const epoll_event &ev) {
    auto &sock_context__ = epoll_data__().sock_context__;
    SOCKET fd = ev.data.fd;
    {
        std::shared_lock<std::shared_mutex> lck(
            *sock_context__[fd].mu_data_buffer_out);
        if (sock_context__[fd].data_buffer_out.empty()) {
            sf_debug("sock", fd, "empty");
            return;
        }
    }
    while (true) {
        byte_array p;
        {
            std::shared_lock<std::shared_mutex> lck(
                *sock_context__[fd].mu_data_buffer_out);
            if (sock_context__[fd].data_buffer_out.empty()) {
                write_finished(fd);
                sock_context__[fd].ev.events &= ~EPOLLOUT;
                epoll_ctl(epoll_data__().epoll_fd, EPOLL_CTL_MOD, fd,
                          &sock_context__[fd].ev);
                sf_debug(fd, "write_finished",
                         sock_context__[fd].data_buffer_out.size());
                break;
            }
            p = sock_context__[fd].data_buffer_out.front();
        }
        sf_debug("pkg size:", p.size());
        auto data_size = p.size();
        auto n = data_size;
        decltype(n) tmp_write;
        bool error_flag = false;
        while (n > 0) {
            tmp_write = static_cast<unsigned long>(
                write(fd, p.data() + data_size - n, n));
            if (tmp_write < n) {
                if (tmp_write == -1 && errno != EAGAIN) {
                    write_error(fd);
                    error_flag = true;
                }
                if (tmp_write > 0) {
                    sf_debug("write", tmp_write);
                    n -= tmp_write;
                }
                break;
            }
            sf_debug("write", tmp_write);
            n -= tmp_write;
        }
        if (error_flag) {
            sf_debug("write error");
            disconnect_sock_filter__(fd);
            epoll_ctl(epoll_data__().epoll_fd, EPOLL_CTL_DEL, fd,
                      &sock_context__[fd].ev);
            close(fd);
            closed(fd);
            sock_context__.erase(fd);
            break;
        } else {
            std::lock_guard<std::shared_mutex> lck(
                *sock_context__[fd].mu_data_buffer_out);
            if (n == 0) {
                sf_debug("pop front");
                sf_debug(fd, "before",
                         sock_context__[fd].data_buffer_out.size());
                sock_context__[fd].data_buffer_out.pop_front();
                sf_debug(fd, "after",
                         sock_context__[fd].data_buffer_out.size());
            } else {
                sock_context__[fd].data_buffer_out.front() = {
                    sock_context__[fd].data_buffer_out.front().end() - n,
                    sock_context__[fd].data_buffer_out.front().end()};
                sf_debug(fd, "write pendding",
                         sock_context__[fd].data_buffer_out.size());
                break;
            }
        }
    }
}

inline epoll_context_t &sf_tcp_server::epoll_data__() const {
    thread_local static epoll_context_t d;
    return d;
}

}    // namespace skyfire