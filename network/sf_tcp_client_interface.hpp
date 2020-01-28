
/**
* @version 1.0.0
* @author skyfire
* @file sf_tcp_client_interface.hpp
*/

#pragma once

#include "network/sf_net_utils.hpp"
#include "sf_tcp_client_interface.h"

namespace skyfire {
inline bool sf_tcp_client_interface::peer_addr(sf_addr_info_t &addr) {
    return skyfire::peer_addr(raw_socket(), addr);
}

inline bool sf_tcp_client_interface::local_addr(sf_addr_info_t &addr) {
    return skyfire::local_addr(raw_socket(), addr);
}
}    // namespace skyfire