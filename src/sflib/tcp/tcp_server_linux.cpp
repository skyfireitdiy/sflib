
#include "sflib/tcp/tcp_server_linux.h"
#include "sflib/tcp/net_utils.h"
#include "sflib/tcp/tcp_server_interface.h"
#include "sflib/tools/define.h"
#include "sflib/tools/finally.h"
#include <memory>

namespace skyfire
{

SOCKET           tcp_server::raw_socket() { return listen_fd__; }
epoll_context_t* tcp_server::find_context__(SOCKET sock) const
{
    auto& epoll_data = epoll_data__();
    {
        std::shared_lock lck(epoll_data.mu_epoll_context__);
        if (epoll_data.sock_context__.count(sock) != 0)
        {
            return &epoll_data;
        }
    }
    {
        std::shared_lock lck(mu_context_pool__);
        for (auto& p : context_pool__)
        {
            {
                std::shared_lock lck(p->mu_epoll_context__);
                if (p->sock_context__.count(sock) != 0)
                {
                    return p;
                }
            }
        }
    }

    return nullptr;
}
bool tcp_server::send(int sock, const byte_array& data)
{

    auto psock_context_data__ = find_context__(sock);
    if (psock_context_data__ == nullptr)
    {

        return false;
    }
    std::shared_lock lck(psock_context_data__->mu_epoll_context__);
    auto&            sock_context__ = psock_context_data__->sock_context__;
    auto             send_data      = data;
    lck.unlock();
    before_raw_send_filter__(sock, send_data);
    lck.lock();
    sock_context__[sock].data_buffer_out.push_back(send_data);
    sock_context__[sock].ev.events |= EPOLLOUT;
    return epoll_ctl(psock_context_data__->epoll_fd, EPOLL_CTL_MOD, sock,
                     &sock_context__[sock].ev)
           != -1;
}
bool tcp_server::send(int sock, int type, const byte_array& data)
{
    auto psock_context_data__ = find_context__(sock);
    if (psock_context_data__ == nullptr)
    {

        return false;
    }
    std::shared_lock lck(psock_context_data__->mu_epoll_context__);
    auto&            sock_context__ = psock_context_data__->sock_context__;
    pkg_header_t     header {};
    header.type   = htonl(type);
    header.length = htonl(data.size());
    make_header_checksum(header);
    auto tmp_data = data;
    lck.unlock();
    before_send_filter__(sock, header, tmp_data);
    auto send_data = make_pkg(header) + tmp_data;
    before_raw_send_filter__(sock, send_data);
    lck.lock();
    sock_context__[sock].data_buffer_out.push_back(send_data);
    sock_context__[sock].ev.events |= EPOLLOUT;
    return epoll_ctl(psock_context_data__->epoll_fd, EPOLL_CTL_MOD, sock,
                     &sock_context__[sock].ev)
           != -1;
}
void tcp_server::close(SOCKET sock)
{
    ::shutdown(sock, SHUT_RDWR);
    ::close(sock);
}
void tcp_server::close()
{

    closed__ = true;
    ::shutdown(listen_fd__, SHUT_RDWR);
    ::close(listen_fd__);
    {
        std::unique_lock
             lck(mu_context_pool__);
        char buf = '0';
        for (auto& p : context_pool__)
        {
            if (::write(p->pipe__[1], &buf, 1) != 1)
            {
            }
            else
            {
            }
        }
    }
    for (auto& p : co_vec__)
    {
        p->join();
    }
    co_vec__.clear();
}
bool tcp_server::listen(const std::string& ip, unsigned short port)
{
    listen_fd__ = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_fd__ == -1)
    {
        return false;
    }
    listen_sock_filter__(listen_fd__);
    if (fcntl(listen_fd__, F_SETFL,
              fcntl(listen_fd__, F_GETFD, 0) | O_NONBLOCK)
        == -1)
    {
        return false;
    }
    int opt = 1;
    if (-1 == setsockopt(listen_fd__, SOL_SOCKET, SO_REUSEADDR, reinterpret_cast<const void*>(&opt), sizeof(opt)))
    {
        return false;
    }
    if (-1 == setsockopt(listen_fd__, SOL_SOCKET, SO_REUSEPORT, reinterpret_cast<const void*>(&opt), sizeof(opt)))
    {
        return false;
    }
    sockaddr_in internet_addr {};
    internet_addr.sin_family      = AF_INET;
    internet_addr.sin_addr.s_addr = inet_addr(ip.c_str());
    internet_addr.sin_port        = htons(port);
    if (::bind(listen_fd__, reinterpret_cast<sockaddr*>(&internet_addr),
               sizeof(sockaddr_in))
        == -1)
    {
        return false;
    }
    if (::listen(listen_fd__, max_tcp_connection) == -1)
    {
        return false;
    }
    co_vec__.emplace_back(std::make_unique<std::thread>(std::function<void()>([this] { work_routine__(true, listen_fd__); })));
    if (config__.manage_clients)
    {
        for (size_t i = 0; i < config__.thread_count; ++i)
        {
            co_vec__.emplace_back(std::make_unique<std::thread>(std::function<void()>([this] { work_routine__(true, listen_fd__); })));
        }
    }
    return true;
}
void tcp_server::work_routine__(bool listen_thread, SOCKET listen_fd)
{

    auto& epoll_data    = epoll_data__();
    epoll_data.epoll_fd = epoll_create(max_tcp_connection);
    auto pipe_ret       = pipe(epoll_data.pipe__);
    sf_finally([this, &epoll_data] {
        std::unique_lock
            lck(mu_context_pool__);
        context_pool__.erase(std::remove(context_pool__.begin(), context_pool__.end(), &epoll_data), context_pool__.end());
    });
    if (pipe_ret != 0)
    {

        return;
    }
    {
        std::lock_guard lck(epoll_data.mu_epoll_context__);
        auto&           sock_context__ = epoll_data.sock_context__;
        if (listen_thread)
        {
            sock_context__[listen_fd]            = sock_data_context_t {};
            sock_context__[listen_fd].ev.events  = EPOLLIN | EPOLLET;
            sock_context__[listen_fd].ev.data.fd = listen_fd;

            if (epoll_ctl(epoll_data.epoll_fd, EPOLL_CTL_ADD, listen_fd, &sock_context__[listen_fd].ev) < 0)
            {

                close(listen_fd);
                return;
            }
        }
        epoll_data.pipe_event__.events  = EPOLLIN | EPOLLET;
        epoll_data.pipe_event__.data.fd = epoll_data.pipe__[0];
        if (epoll_ctl(epoll_data.epoll_fd, EPOLL_CTL_ADD, epoll_data.pipe__[0], &epoll_data.pipe_event__) < 0)
        {

            ::close(epoll_data.pipe__[0]);
            ::close(epoll_data.pipe__[1]);
            return;
        }
    }
    {
        std::unique_lock
            lck(mu_context_pool__);
        context_pool__.push_back(&epoll_data);
    }
    // 需要为管道留一个位置
    std::vector<epoll_event> evs(max_tcp_connection + 1);
    while (!closed__)
    {
        int wait_fds = 0;
        if ((wait_fds = epoll_wait(epoll_data.epoll_fd, evs.data(),
                                   max_tcp_connection, -1))
            == -1)
        {
            if (errno == EINTR)
            {
                continue;
            }
            break;
        }
        if (wait_fds == 0)
        {
            continue;
        }

        auto listen_err = false;
        for (auto i = 0; i < wait_fds; ++i)
        {
            if (evs[i].data.fd == listen_fd__ && (evs[i].events & EPOLLIN))
            {

                if (!handle_accept__())
                {

                    listen_err = true;
                }
            }
            else if (evs[i].events & EPOLLIN)
            {
                if (evs[i].data.fd != epoll_data.pipe__[0])
                {
                    handle_read__(evs[i]);
                }
                else
                {
                    char buf;
                    if (::read(epoll_data.pipe__[0], &buf, 1) != 1)
                    {
                    }
                    ::close(epoll_data.pipe__[0]);
                    ::close(epoll_data.pipe__[1]);
                }
            }
            else if (evs[i].events & EPOLLOUT)
            {
                handle_write__(evs[i]);
            }
        }
        if (listen_err)
        {

            break;
        }
        if (closed__)
        {
            return;
        }
    }
}
tcp_server::~tcp_server() { close(); }
bool tcp_server::in_dispatch__(SOCKET fd)
{

    auto&           epoll_data = epoll_data__();
    std::lock_guard lck(epoll_data.mu_epoll_context__);
    auto&           sock_context__ = epoll_data.sock_context__;
    sock_context__[fd]             = sock_data_context_t {};
    sock_context__[fd].ev.events   = EPOLLIN | EPOLLET;
    sock_context__[fd].ev.data.fd  = fd;

    if (epoll_ctl(epoll_data.epoll_fd, EPOLL_CTL_ADD, fd,
                  &sock_context__[fd].ev)
        < 0)
    {

        close(fd);
        return false;
    }
    return true;
}
bool tcp_server::handle_accept__()
{
    int         conn_fd = 0;
    sockaddr_in client_addr {};
    socklen_t   len = sizeof(client_addr);

    while ((conn_fd = ::accept4(listen_fd__, (struct sockaddr*)&client_addr, &len, SOCK_NONBLOCK)) > 0)
    {
        new_connection_filter__(conn_fd);
        if (config__.manage_clients)
        {
            if (fcntl(conn_fd, F_SETFL,
                      fcntl(conn_fd, F_GETFD, 0) | O_NONBLOCK)
                == -1)
            {

                close(conn_fd);
                return true;
            }
            if (!in_dispatch__(conn_fd))
            {
                return true;
            }
        }

        new_connection(conn_fd);
    }
    if (errno == EAGAIN || errno == EINTR)
    {

        return true;
    }
    else
    {

        {
            auto&           epoll_data = epoll_data__();
            std::lock_guard lck(epoll_data.mu_epoll_context__);
            epoll_ctl(epoll_data.epoll_fd, EPOLL_CTL_DEL, listen_fd__,
                      &epoll_data.sock_context__[listen_fd__].ev);
            epoll_data.sock_context__.erase(listen_fd__);
        }
        return false;
    }
}
sf_error tcp_server::recv(SOCKET sock, byte_array& data, int length)
{
    auto& epoll_data     = epoll_data__();
    auto& sock_context__ = epoll_data.sock_context__;

    auto count_read = static_cast<int>(::recv(sock, data.data(), default_buffer_size, MSG_NOSIGNAL));

    if (count_read <= 0)
    {

        // EWOULDBLOCK == EAGAIN
        if ((errno == EAGAIN || errno == EINTR /* || errno == EWOULDBLOCK */) && count_read < 0)
        {
            return sf_error { exception(err_finished, "read finished") };
        }
        else
        {
            disconnect_sock_filter__(sock);
            {
                std::lock_guard lck(epoll_data.mu_epoll_context__);
                epoll_ctl(epoll_data.epoll_fd, EPOLL_CTL_DEL,
                          sock, &sock_context__[sock].ev);
                sock_context__.erase(sock);
            }
            closed(sock);
            close(sock);
            return sf_error { exception(err_disconnect, "connection closed") };
        }
    }
    data.resize(static_cast<unsigned long>(count_read));
    return sf_error {};
}
void tcp_server::handle_read__(const epoll_event& ev)
{
    if (!config__.manage_read)
    {
        ready_read(ev.data.fd);
        return;
    }
    byte_array   recv_buf(default_buffer_size);
    pkg_header_t header {};
    auto&        epoll_data     = epoll_data__();
    auto&        sock_context__ = epoll_data.sock_context__;
    while (true)
    {
        auto e = recv(ev.data.fd, recv_buf);
        if (e.exp().code() == err_finished)
        {

            break;
        }
        if (e.exp().code() == err_disconnect)
        {

            break;
        }
        after_raw_recv_filter__(ev.data.fd, recv_buf);
        if (config__.raw)
        {

            raw_data_coming(ev.data.fd, recv_buf);
        }
        else
        {
            std::shared_lock lck(epoll_data.mu_epoll_context__);
            sock_context__[ev.data.fd].data_buffer_in.insert(
                sock_context__[ev.data.fd].data_buffer_in.end(),
                recv_buf.begin(), recv_buf.end());
            size_t read_pos = 0;
            while (sock_context__[ev.data.fd].data_buffer_in.size() - read_pos >= sizeof(pkg_header_t))
            {
                memmove(
                    &header,
                    sock_context__[ev.data.fd].data_buffer_in.data() + read_pos,
                    sizeof(header));
                if (!check_header_checksum(header))
                {
                    lck.unlock();
                    disconnect_sock_filter__(ev.data.fd);
                    lck.lock();
                    close(ev.data.fd);
                    closed(ev.data.fd);
                    break;
                }
                header.length = ntohl(header.length);
                header.type   = ntohl(header.type);
                if (sock_context__[ev.data.fd].data_buffer_in.size() - read_pos - sizeof(header) >= header.length)
                {
                    byte_array data = { byte_array(
                        sock_context__[ev.data.fd].data_buffer_in.begin() + read_pos + sizeof(header),
                        sock_context__[ev.data.fd].data_buffer_in.begin() + read_pos + sizeof(header) + header.length) };
                    read_pos += sizeof(header) + header.length;
                    lck.unlock();
                    after_recv_filter__(ev.data.fd, header, data);
                    data_coming(ev.data.fd, header, data);
                    lck.lock();
                }
                else
                {
                    break;
                }
            }
            if (read_pos != 0)
            {
                sock_context__[ev.data.fd].data_buffer_in.erase(
                    sock_context__[ev.data.fd].data_buffer_in.begin(),
                    sock_context__[ev.data.fd].data_buffer_in.begin() + read_pos);
            }
        }
    }
}
void tcp_server::handle_write__(const epoll_event& ev)
{
    auto&  epoll_data     = epoll_data__();
    auto&  sock_context__ = epoll_data.sock_context__;
    SOCKET fd             = ev.data.fd;
    {
        std::shared_lock lck(epoll_data.mu_epoll_context__);
        if (sock_context__[fd].data_buffer_out.empty())
        {

            return;
        }
    }
    while (true)
    {
        byte_array p;
        {
            std::shared_lock lck(epoll_data.mu_epoll_context__);
            if (sock_context__[fd].data_buffer_out.empty())
            {
                lck.unlock();
                write_finished(fd);
                lck.lock();
                sock_context__[fd].ev.events &= ~EPOLLOUT;
                epoll_ctl(epoll_data.epoll_fd, EPOLL_CTL_MOD, fd,
                          &sock_context__[fd].ev);
                break;
            }
            p = sock_context__[fd].data_buffer_out.front();
        }

        auto data_size = p.size();
        auto n         = data_size;
        int  tmp_write;
        bool error_flag = false;
        while (n > 0)
        {
            tmp_write = static_cast<unsigned long>(
                ::send(fd, p.data() + data_size - n, n, MSG_NOSIGNAL));
            if (tmp_write == -1 && errno != EAGAIN)
            {
                write_error(fd);
                error_flag = true;
                break;
            }

            n -= tmp_write;
        }
        if (error_flag)
        {

            disconnect_sock_filter__(fd);
            std::lock_guard lck(epoll_data.mu_epoll_context__);
            epoll_ctl(epoll_data.epoll_fd, EPOLL_CTL_DEL, fd,
                      &sock_context__[fd].ev);
            close(fd);
            closed(fd);
            sock_context__.erase(fd);
            break;
        }
        else
        {
            std::shared_lock lck(epoll_data.mu_epoll_context__);
            if (n == 0)
            {
                sock_context__[fd].data_buffer_out.pop_front();
            }
            else
            {
                sock_context__[fd].data_buffer_out.front() = {
                    sock_context__[fd].data_buffer_out.front().end() - n,
                    sock_context__[fd].data_buffer_out.front().end()
                };
                break;
            }
        }
    }
}

thread_local epoll_context_t __epoll_context__;

epoll_context_t& tcp_server::epoll_data__() const
{
    return __epoll_context__;
}
} // namespace skyfire