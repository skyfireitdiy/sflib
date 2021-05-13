#include "net_utils.h"
#include "error.h"
#include "utils.h"

namespace skyfire
{
sock_addr_result parse_net_addr(const std::string& addr)
{
    auto result = string_split(addr, ":");
    if (result.size() != 3) // 协议、host、端口
    {
        return { sf_error { { err_parse, "parse error:" + addr } }, {} };
    }
    sock_addr s_addr;
    if (result[0] == "tcp")
    {
        s_addr.type = sock_type::tcp;
    }
    else if (result[0] == "udp")
    {
        s_addr.type = sock_type::udp;
    }
    else if (result[0] == "unix")
    {
        s_addr.type = sock_type::unix_;
    }
    else
    {
        return {
            sf_error { { err_unsupported_type, "unsupported type:" + addr } }, {}
        };
    }
    s_addr.host = result[1];
    s_addr.port = string_to_long_double(result[2]);
    return { {}, s_addr };
}
} // namespace skyfire
