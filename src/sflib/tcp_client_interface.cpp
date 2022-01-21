
#include "sflib/net_utils.h"
#include "sflib/tcp_client_interface.h"
namespace skyfire
{
bool tcp_client_interface::peer_addr(addr_info_t& addr)
{
    return skyfire::peer_addr(raw_socket(), addr);
}
bool tcp_client_interface::local_addr(addr_info_t& addr)
{
    return skyfire::local_addr(raw_socket(), addr);
}
} // namespace skyfire