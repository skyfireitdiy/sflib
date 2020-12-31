
#pragma once
#include "sf_net_utils.hpp"
#include "sf_tcp_client_interface.h"
namespace skyfire
{
inline bool tcp_client_interface::peer_addr(addr_info_t& addr)
{
    return skyfire::peer_addr(raw_socket(), addr);
}
inline bool tcp_client_interface::local_addr(addr_info_t& addr)
{
    return skyfire::local_addr(raw_socket(), addr);
}
} // namespace skyfire