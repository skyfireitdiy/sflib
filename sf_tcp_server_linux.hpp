
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

#include "sf_tcp_server_linux.h"
#include "sf_define.h"
#include "sf_net_utils.hpp"
#include "sf_tcp_server_interface.hpp"
#include <sys/sysinfo.h>

namespace skyfire
{
    inline sf_tcp_server::sf_tcp_server(bool raw)
    {
        raw__ = raw;
    }

    inline SOCKET sf_tcp_server::get_raw_socket()
    {
        return listen_fd__;
    }

    inline bool sf_tcp_server::send(int sock, const byte_array &data)
    {
        if(sock_context__.count(sock) == 0)
            return false;

        auto send_data = data;
        before_raw_send_filter__(sock, send_data);

        std::lock_guard<std::mutex> lck(*sock_context__[sock].mu_out_buffer);
        sock_context__[sock].data_buffer_out.push_back(send_data);

        sock_context__[sock].ev.events |= EPOLLOUT;
        return epoll_ctl(epoll_fd__, EPOLL_CTL_MOD, sock, &sock_context__[sock].ev) != -1;
    }

    inline bool sf_tcp_server::send(int sock, int type, const byte_array &data)
    {
        if(sock_context__.count(sock) == 0)
            return false;
        sf_pkg_header_t header{};
        header.type = type;
        header.length = data.size();
        make_header_checksum(header);
        auto tmp_data = data;
        before_send_filter__(sock,header, tmp_data);
        auto send_data = make_pkg(header) + tmp_data;

        std::lock_guard<std::mutex> lck(*sock_context__[sock].mu_out_buffer);
        sock_context__[sock].data_buffer_out.push_back(send_data);

        sock_context__[sock].ev.events |= EPOLLOUT;
        return epoll_ctl(epoll_fd__, EPOLL_CTL_MOD, sock, &sock_context__[sock].ev) != -1;

    }

    inline void sf_tcp_server::close(SOCKET sock)
    {
        ::shutdown(sock, SHUT_RDWR);
        ::close(sock);
    }

    inline void sf_tcp_server::close()
    {
        shutdown(listen_fd__, SHUT_RDWR);
        ::close(listen_fd__);
        listen_fd__ = -1;
        for(auto &p:sock_context__)
        {
            epoll_ctl(epoll_fd__, EPOLL_CTL_DEL, p.first,
                      &p.second.ev);
        }
        sock_context__.clear();
    }

    inline bool sf_tcp_server::listen(const std::string &ip, unsigned short port)
    {
        listen_fd__ = socket(AF_INET, SOCK_STREAM, 0);
        if (listen_fd__ == -1)
        {
            return false;
        }

        listen_sock_filter__(listen_fd__);

        if (fcntl(listen_fd__, F_SETFL, fcntl(listen_fd__, F_GETFD, 0) | O_NONBLOCK) == -1)
        {
            return false;
        }

        int opt = 1;
        if (-1 == setsockopt(listen_fd__, SOL_SOCKET, SO_REUSEADDR,
                             reinterpret_cast<const void *>(&opt), sizeof(opt)))
        {
            return false;
        }

        sockaddr_in internet_addr{};
        internet_addr.sin_family = AF_INET;
        internet_addr.sin_addr.s_addr = inet_addr(ip.c_str());
        internet_addr.sin_port = htons(port);

        if (::bind(listen_fd__, reinterpret_cast<sockaddr *>(&internet_addr), sizeof(sockaddr_in)) == -1)
        {
            return false;
        }

        if (::listen(listen_fd__, max_tcp_connection) == -1)
        {
            return false;
        }


        epoll_fd__ = epoll_create(max_tcp_connection);    //!> create

        sock_context__[listen_fd__] = sock_data_context_t{};
        sock_context__[listen_fd__].ev.events = EPOLLIN | EPOLLET;
        sock_context__[listen_fd__].ev.data.fd = listen_fd__;


        if (epoll_ctl(epoll_fd__, EPOLL_CTL_ADD, listen_fd__, &sock_context__[listen_fd__].ev) < 0)
        {
            return false;
        }

        thread_count__ = get_nprocs() * 2 + 2;

        auto work_thread = [=]
        {
            std::vector<epoll_event> evs(max_tcp_connection);
            byte_array recv_buf(SF_DEFAULT_BUFFER_SIZE);
            sf_pkg_header_t header{};
            while (true)
            {
                int wait_fds = 0;
                sf_debug("wait epoll event", sock_context__.size());
                if ((wait_fds = epoll_wait(epoll_fd__, evs.data(), max_tcp_connection, 1000)) == -1)
                {
                    if (errno == EINTR)
                    {
                        continue;
                    }
                    break;
                }

                if(wait_fds==0){
                    continue;
                }

                sf_debug("new epoll event", wait_fds);

                for (auto i = 0; i < wait_fds; ++i)
                {
                    if (evs[i].data.fd == listen_fd__ && (evs[i].events & EPOLLIN))
                    {
                        int conn_fd = 0;
                        sockaddr_in client_addr{};
                        socklen_t len = sizeof(client_addr);
                        while ((conn_fd = accept(listen_fd__, (struct sockaddr *) &client_addr, &len)) >
                               0)
                        {
                            new_connection_filter__(conn_fd);

                            if (fcntl(conn_fd, F_SETFL, fcntl(conn_fd, F_GETFD, 0) | O_NONBLOCK) == -1)
                            {
                                sf_debug("set no block error");
                                close(conn_fd);
                                continue;
                            }

                            {
                                sock_context__[conn_fd] = sock_data_context_t{};
                                sock_context__[conn_fd].ev.events = EPOLLIN | EPOLLET;
                                sock_context__[conn_fd].ev.data.fd = conn_fd;

                                if (epoll_ctl(epoll_fd__, EPOLL_CTL_ADD, conn_fd,
                                              &sock_context__[conn_fd].ev) <
                                    0)
                                {
                                    sf_debug("add to epoll error");
                                    close(conn_fd);
                                    continue;
                                }
                            }

                            sf_debug("new connection", conn_fd);
                            new_connection(conn_fd);
                        }

                        if (errno == EAGAIN || errno == EINTR)
                        {
                            sf_debug("accept finished");
                            continue;
                        } else
                        {
                            sf_debug("accept error");
                            {
                                epoll_ctl(epoll_fd__, EPOLL_CTL_DEL, listen_fd__,
                                          &sock_context__[listen_fd__].ev);
                                sock_context__.erase(listen_fd__);
                            }
                            break;
                        }

                    } else if (evs[i].events & EPOLLIN)
                    {
                        std::unique_lock<std::mutex> lck(*sock_context__[evs[i].data.fd].read_lock);
                        while (true)
                        {
                            recv_buf.resize(SF_DEFAULT_BUFFER_SIZE);
                            sf_debug("start read");
                            auto count_read = static_cast<int>(read(evs[i].data.fd, recv_buf.data(),
                                                                    SF_DEFAULT_BUFFER_SIZE));
                            sf_debug("read", count_read);

                            if (count_read <= 0)
                            {
                                if ((errno == EAGAIN || errno == EINTR) && count_read < 0)
                                {
                                    sf_debug("read finished", errno);
                                    break;
                                } else
                                {
                                    disconnect_sock_filter__(evs[i].data.fd);
                                    closed(static_cast<SOCKET>(evs[i].data.fd));
                                    close(evs[i].data.fd);
                                    {
                                        epoll_ctl(epoll_fd__, EPOLL_CTL_DEL, evs[i].data.fd,
                                                  &sock_context__[evs[i].data.fd].ev);
                                        sock_context__.erase(evs[i].data.fd);
                                    }
                                    sf_debug("read error / connection closed");
                                    break;
                                }
                            }

                            recv_buf.resize(static_cast<unsigned long>(count_read));
                            if (raw__)
                            {
                                sf_debug("raw data", recv_buf.size());
                                after_raw_recv_filter__(static_cast<SOCKET>(evs[i].data.fd), recv_buf);
                                raw_data_coming(static_cast<SOCKET>(evs[i].data.fd), recv_buf);
                                sf_debug("after reslove");
                            } else
                            {
                                sock_context__[evs[i].data.fd].data_buffer_in.insert(
                                        sock_context__[evs[i].data.fd].data_buffer_in.end(),
                                        recv_buf.begin(),
                                        recv_buf.end());
                                size_t read_pos = 0;
                                while (sock_context__[evs[i].data.fd].data_buffer_in.size() - read_pos >=
                                       sizeof(sf_pkg_header_t))
                                {
                                    memmove(&header,
                                            sock_context__[evs[i].data.fd].data_buffer_in.data() + read_pos,
                                            sizeof(header));
                                    if (!check_header_checksum(header))
                                    {
                                        disconnect_sock_filter__(evs[i].data.fd);
                                        close(evs[i].data.fd);
                                        closed(static_cast<SOCKET>(evs[i].data.fd));
                                        break;
                                    }
                                    if (sock_context__[evs[i].data.fd].data_buffer_in.size() - read_pos -
                                        sizeof(header) >=
                                        header.length)
                                    {
                                        byte_array data = {byte_array(
                                                sock_context__[evs[i].data.fd].data_buffer_in.begin() +
                                                read_pos +
                                                sizeof(header),
                                                sock_context__[evs[i].data.fd].data_buffer_in.begin() +
                                                read_pos +
                                                sizeof(header) + header.length)};
                                        read_pos += sizeof(header) + header.length;

                                        after_recv_filter__(static_cast<SOCKET>(evs[i].data.fd), header, data);

                                        data_coming(
                                                static_cast<SOCKET>(evs[i].data.fd), header,
                                                data
                                        );

                                    } else
                                    {
                                        break;
                                    }
                                }
                                if (read_pos != 0)
                                {
                                    sock_context__[evs[i].data.fd].data_buffer_in.erase(
                                            sock_context__[evs[i].data.fd].data_buffer_in.begin(),
                                            sock_context__[evs[i].data.fd].data_buffer_in.begin() + read_pos);
                                }
                            }
                        }
                    } else if (evs[i].events & EPOLLOUT)
                    {
                        SOCKET fd = evs[i].data.fd;
                        if (sock_context__[fd].data_buffer_out.empty())
                        {
                            break;
                        }
                        std::lock_guard<std::mutex> lck(*sock_context__[fd].mu_out_buffer);
                        while (true)
                        {
                            if (sock_context__[fd].data_buffer_out.empty())
                                break;
                            auto p = sock_context__[fd].data_buffer_out.front();
                            auto data_size = p.size();
                            auto n = data_size;
                            decltype(n) tmp_write;
                            bool error_flag = false;
                            while (n > 0)
                            {
                                tmp_write = static_cast<unsigned long>(write(fd, p.data() + data_size - n, n));
                                if (tmp_write < n)
                                {
                                    if (tmp_write == -1 && errno != EAGAIN)
                                    {
                                        write_error(fd);
                                        error_flag = true;
                                    }
                                    break;
                                }
                                n -= tmp_write;
                            }
                            if (error_flag)
                            {
                                sf_debug("write error");
                                disconnect_sock_filter__(fd);
                                close(fd);
                                closed(fd);
                                epoll_ctl(epoll_fd__, EPOLL_CTL_DEL, fd, &sock_context__[fd].ev);
                                sock_context__.erase(fd);
                                break;
                            } else
                            {
                                if (n == 0)
                                {
                                    sock_context__[fd].data_buffer_out.pop_front();
                                    write_finished(fd);
                                    sock_context__[fd].ev.events &= ~EPOLLOUT;
                                    epoll_ctl(epoll_fd__, EPOLL_CTL_MOD, fd, &sock_context__[fd].ev);
                                    sf_debug("write_finished");
                                } else
                                {
                                    sf_debug("write pendding");
                                    sock_context__[fd].data_buffer_out.front() = {
                                            sock_context__[fd].data_buffer_out.front().begin() + n,
                                            sock_context__[fd].data_buffer_out.front().end()};
                                    break;
                                }
                            }
                        }
                    }
                }
            }
        };
        for (int i = 0; i < thread_count__; ++i)
        {
            std::thread(work_thread).detach();
        }
        return true;
    }

    inline std::shared_ptr<sf_tcp_server> sf_tcp_server::make_server(bool raw)
    {
        return std::make_shared<sf_tcp_server>(raw);
    }

    inline sf_tcp_server::~sf_tcp_server()
    {
        close();
    }
}