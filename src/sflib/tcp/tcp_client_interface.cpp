
#include "sflib/tcp/tcp_client_interface.h"
#include "sflib/tcp/net_utils.h"

namespace skyfire
{
bool tcp_client_interface::peer_addr(addr_info_t &addr)
{
    return skyfire::peer_addr(raw_socket(), addr);
}
bool tcp_client_interface::local_addr(addr_info_t &addr)
{
    return skyfire::local_addr(raw_socket(), addr);
}
} // namespace skyfire