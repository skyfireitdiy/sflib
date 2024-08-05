
#include "sflib/tcp/net_utils.h"
#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/epoll.h>
#include <sys/resource.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
namespace skyfire
{
bool peer_addr(SOCKET sock, addr_info_t &addr)
{
    sockaddr_in sock_addr {};
    memset(&sock_addr, 0, sizeof(sock_addr));
    socklen_t len = sizeof(sock_addr);
    if (getpeername(sock, reinterpret_cast<sockaddr *>(&sock_addr), &len) != 0)
    {
        return false;
    }
    addr.ip = inet_ntoa(sock_addr.sin_addr);
    addr.port = ntohs(sock_addr.sin_port);
    return true;
}
bool local_addr(SOCKET sock, addr_info_t &addr)
{
    sockaddr_in sock_addr {};
    memset(&sock_addr, 0, sizeof(sock_addr));
    socklen_t len = sizeof(sock_addr);
    if (getsockname(sock, reinterpret_cast<sockaddr *>(&sock_addr), &len) != 0)
    {
        return false;
    }
    addr.ip = inet_ntoa(sock_addr.sin_addr);
    addr.port = ntohs(sock_addr.sin_port);
    return true;
}
ip_list_result resolve_dns(const std::string &host)
{
    auto ip_list = gethostbyname(host.c_str());
    if (ip_list == nullptr)
    {
        return {{err_resolve_dns, "Resolve dns error", h_errno}, {}};
    }
    std::vector<std::string> ip_list_vec;
    for (auto i = 0; ip_list->h_addr_list[i]; ++i)
    {
        ip_list_vec.push_back(inet_ntoa(*reinterpret_cast<in_addr *>(ip_list->h_addr_list[i])));
    }
    return {sf_error {}, ip_list_vec};
}
}