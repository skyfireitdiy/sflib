#include "error.h"
#include "multi_value.h"
#include <bits/stdc++.h>

namespace skyfire
{
enum class sock_type
{
    tcp,
    udp,
    unix_, // unix 宏已经被定义了
};

struct sock_addr
{
    sock_type      type;
    std::string    host;
    unsigned short port;
};

using sock_addr_result = multi_value<sf_error, sock_addr>;

} // namespace skyfire
