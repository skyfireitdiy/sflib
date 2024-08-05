
#include "sflib/tcp/tcp_client_linux.h"
#include "sflib/core/type.h"
#include "sflib/object/object.h"
#include "sflib/tcp/tcp_client_interface.h"
#include "sflib/tcp/tcp_utils.h"
#include "sflib/tools/nocopy.h"

namespace skyfire
{
tcp_client::tcp_client(bool raw)
{
    sock__ = socket(AF_INET, SOCK_STREAM, 0);
    if (sock__ == -1)
    {
        inited__ = false;
        return;
    }
    int opt = 1;
    if (-1 == setsockopt(sock__, SOL_SOCKET, SO_REUSEADDR, reinterpret_cast<const void*>(&opt), sizeof(opt)))
    {
        inited__ = false;
        return;
    }
    inited__ = true;
    raw__    = raw;
}
tcp_client::tcp_client(SOCKET sock, bool raw)
{
    sock__        = sock;
    inited__      = true;
    raw__         = raw;
    loop_thread__ = std::async([this] { recv_routine__(); });
}
SOCKET tcp_client::raw_socket() { return sock__; }
bool   tcp_client::bind(const std::string& ip, unsigned short port)
{
    sockaddr_in address {};
    address.sin_family      = AF_INET;
    address.sin_addr.s_addr = inet_addr(ip.c_str());
    address.sin_port        = htons(port);
    return -1 != ::bind(sock__, reinterpret_cast<sockaddr*>(&address), sizeof(address));
}
tcp_client::~tcp_client() { close(); }
sf_error tcp_client::connect_to_server(const std::string& host,
                                       unsigned short     port)
{
    if (!inited__)
        return { err_uninit, "Uninit" };
    sockaddr_in address {};
    auto        host_list = resolve_dns(host);
    if (!sf_error(host_list))
    {
        return sf_error(host_list);
    }
    for (auto& ip : std::vector<std::string>(host_list))
    {
        address.sin_family      = AF_INET;
        address.sin_addr.s_addr = inet_addr(ip.c_str());
        address.sin_port        = htons(port);
        if (::connect(sock__, reinterpret_cast<const sockaddr*>(&address),
                      sizeof(address))
            != 0)
        {
            continue;
        }
        loop_thread__ = std::async([this] { recv_routine__(); });
        return sf_error {};
    }
    return sf_error { err_connect, "Connect failed" };
}
bool tcp_client::send(int type, const byte_array& data)
{
    if (!inited__)
        return false;
    pkg_header_t header {};
    header.type   = htonl(type);
    header.length = htonl(data.size());
    make_header_checksum(header);
    auto ret = ::write(sock__, make_pkg(header).data(), sizeof(header));
    if (ret != sizeof(header))
        return false;
    return ::write(sock__, data.data(), data.size()) == static_cast<ssize_t>(data.size());
}
bool tcp_client::send(const byte_array& data)
{
    if (!inited__)
        return false;
    return ::write(sock__, data.data(), data.size()) == static_cast<ssize_t>(data.size());
}
void tcp_client::close()
{
    if (!inited__)
        return;
    shutdown(sock__, SHUT_RDWR);
    ::close(sock__);
    sock__ = -1;
}
void tcp_client::recv_routine__()
{
    byte_array   recv_buffer(default_buffer_size);
    byte_array   data;
    pkg_header_t header {};
    while (true)
    {
        auto len = ::read(sock__, recv_buffer.data(), default_buffer_size);
        if (len <= 0)
        {
            closed();
            break;
        }
        if (raw__)
        {
            raw_data_coming(
                byte_array(recv_buffer.begin(), recv_buffer.begin() + len));
        }
        else
        {
            data.insert(data.end(), recv_buffer.begin(),
                        recv_buffer.begin() + len);
            size_t read_pos = 0;
            while (data.size() - read_pos >= sizeof(pkg_header_t))
            {
                std::memmove(&header, data.data() + read_pos, sizeof(header));
                if (!check_header_checksum(header))
                {
                    close();
                    return;
                }
                header.length = ntohl(header.length);
                header.type   = ntohl(header.type);
                if (data.size() - read_pos - sizeof(header) >= header.length)
                {
                    byte_array recv_data = {
                        data.begin() + static_cast<long>(read_pos) + sizeof(header),
                        data.begin() + static_cast<long>(read_pos) + sizeof(header) + static_cast<long>(header.length)
                    };
                    data_coming(header, recv_data);
                    read_pos += sizeof(header) + header.length;
                }
                else
                {
                    break;
                }
            }
            if (read_pos != 0)
            {
                data.erase(data.begin(),
                           data.begin() + static_cast<long>(read_pos));
            }
        }
    }
}
} // namespace skyfire
