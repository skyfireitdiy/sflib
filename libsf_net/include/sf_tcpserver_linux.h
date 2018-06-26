#include <unistd.h>
#include <sys/types.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <sys/resource.h>
#include <memory>
#include <string>
#include "sf_tcputils.h"
#include "sf_nocopy.h"
#include "sf_object.h"
#include "sf_type.h"
#include "sf_range.h"
#include "sf_serialize.h"


namespace skyfire
{
    class sf_tcpserver : public sf_nocopy<sf_object>
    {

    SF_REG_SIGNAL(new_connection, SOCKET)
    SF_REG_SIGNAL(data_coming, SOCKET, const pkg_header_t&, const byte_array&)
    SF_REG_SIGNAL(raw_data_coming, SOCKET, const byte_array&)
    SF_REG_SIGNAL(closed, SOCKET)

    private:
        int listen_fd__ = -1;
        std::map<int, byte_array> sock_data_buffer__;
        int cur_fd_count__ = -1;
        int epoll_fd__ = -1;
        bool raw__ = false;
        epoll_event evs[SOMAXCONN];
        epoll_event ev;

    public:
        sf_tcpserver(bool raw = false)
        {
            raw__ = raw;
        }

        ~sf_tcpserver()
        {
            close();
        }

        static std::shared_ptr<sf_tcpserver> make_server(bool raw = false)
        {
            return std::make_shared<sf_tcpserver>(raw);
        }

        bool listen(const std::string &ip, unsigned short port)
        {
            listen_fd__ = socket(AF_INET, SOCK_STREAM, 0);
            if (listen_fd__ == -1)
            {
                return false;
            }
            if (fcntl(listen_fd__, F_SETFL, fcntl(listen_fd__, F_GETFD, 0) | O_NONBLOCK) == -1)
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

            if (::listen(listen_fd__, SOMAXCONN) == -1)
            {
                return false;
            }

            epoll_fd__ = epoll_create(SOMAXCONN);    //!> create
            ev.events = EPOLLIN | EPOLLET;      //!> accept Read!
            ev.data.fd = listen_fd__;                 //!> 将listen_fd 加入
            if (epoll_ctl(epoll_fd__, EPOLL_CTL_ADD, listen_fd__, &ev) < 0)
            {
                return false;
            }

            cur_fd_count__ = 1;

            std::thread([=]
                        {

                            byte_array recv_buf(SF_NET_BUFFER_SIZE);
                            pkg_header_t header{};
                            while (true)
                            {
                                int wait_fds = 0;
                                if ((wait_fds = epoll_wait(epoll_fd__, evs, cur_fd_count__, -1)) == -1)
                                {
                                    break;
                                }

                                for (auto i = 0; i < wait_fds; ++i)
                                {
                                    if (evs[i].data.fd == listen_fd__ && cur_fd_count__ < SOMAXCONN)
                                    {
                                        int conn_fd = 0;
                                        sockaddr_in client_addr{};
                                        socklen_t len = sizeof(client_addr);
                                        if ((conn_fd = accept(listen_fd__, (struct sockaddr *) &client_addr, &len)) ==
                                            -1)
                                        {
                                            break;
                                        }
                                        ev.events = EPOLLIN | EPOLLET;
                                        ev.data.fd = conn_fd;
                                        if (epoll_ctl(epoll_fd__, EPOLL_CTL_ADD, conn_fd, &ev) < 0)
                                        {
                                            ::close(conn_fd);
                                            continue;
                                        }
                                        ++cur_fd_count__;
                                        std::thread([=]
                                                    {
                                                        new_connection(conn_fd);
                                                    }).detach();
                                        continue;
                                    }
                                    recv_buf.resize(SF_NET_BUFFER_SIZE);
                                    auto count_read = static_cast<int>(read(evs[i].data.fd, recv_buf.data(), SF_NET_BUFFER_SIZE));
                                    if (count_read <= 0)
                                    {
                                        closed(static_cast<SOCKET>(evs[i].data.fd));
                                        ::close(evs[i].data.fd);
                                        epoll_ctl(epoll_fd__, EPOLL_CTL_DEL, evs[i].data.fd, &ev);
                                        --cur_fd_count__;
                                        std::thread([=]()
                                                    {
                                                        ::close(evs[i].data.fd);
                                                    }).detach();
                                        continue;
                                    }


                                    recv_buf.resize(static_cast<unsigned long>(count_read));
                                        if(raw__)
                                        {
                                            raw_data_coming(static_cast<SOCKET>(evs[i].data.fd), recv_buf);
                                        }
                                        else
                                        {
                                            sock_data_buffer__[evs[i].data.fd].insert(
                                                    sock_data_buffer__[evs[i].data.fd].end(),
                                                    recv_buf.begin(),
                                                    recv_buf.end());
                                            size_t read_pos = 0;
                                            while (sock_data_buffer__[evs[i].data.fd].size() - read_pos >=
                                                   sizeof(pkg_header_t))
                                            {
                                                memmove(&header,
                                                        sock_data_buffer__[evs[i].data.fd].data() + read_pos,
                                                        sizeof(header));
                                                if(!check_header_checksum(header))
                                                {
                                                    close(evs[i].data.fd);
                                                    closed(static_cast<SOCKET>(evs[i].data.fd));
                                                    break;
                                                }
                                                if (sock_data_buffer__[evs[i].data.fd].size() - read_pos - sizeof(header) >=
                                                    header.length)
                                                {
                                                    std::thread([=](SOCKET sock, const pkg_header_t& tmp_header, const byte_array& pkg_data)
                                                                {
                                                                    data_coming(sock, tmp_header, pkg_data);
                                                                },
                                                                static_cast<SOCKET>(evs[i].data.fd), header,
                                                                byte_array(
                                                                        sock_data_buffer__[evs[i].data.fd].begin() +
                                                                        read_pos +
                                                                        sizeof(header),
                                                                        sock_data_buffer__[evs[i].data.fd].begin() +
                                                                        read_pos +
                                                                        sizeof(header) + header.length)
                                                    ).detach();
                                                    read_pos += sizeof(header) + header.length;
                                                } else
                                                {
                                                    break;
                                                }
                                            }
                                            if (read_pos != 0)
                                            {
                                                sock_data_buffer__[evs[i].data.fd].erase(
                                                        sock_data_buffer__[evs[i].data.fd].begin(),
                                                        sock_data_buffer__[evs[i].data.fd].begin() + read_pos);
                                            }
                                        }

                                }
                            }
                        }).detach();
            return true;
        }


        void close()
        {
            ::close(listen_fd__);
            listen_fd__ = -1;
            sock_data_buffer__.clear();
        }
        
        void close(SOCKET sock)
        {
            ::close(sock);
        }

        bool send(int sock, int type, const byte_array &data)
        {
            pkg_header_t header{};
            header.type = type;
            header.length = data.size();
            make_header_checksum(header);
            auto send_data = make_pkg(header) + data;
            return write(sock, send_data.data(), send_data.size()) == send_data.size();
        }

        bool send(int sock, const byte_array &data)
        {
            auto send_data = data;
            return write(sock, send_data.data(), send_data.size()) == send_data.size();
        }

        SOCKET get_raw_socket()
        {
            return listen_fd__;
        }
    };
}