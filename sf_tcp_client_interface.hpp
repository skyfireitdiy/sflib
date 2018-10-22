
/**
* @version 1.0.0
* @author skyfire
* @mail skyfireitdiy@hotmail.com
* @see http://github.com/skyfireitdiy/sflib
* @file sf_tcp_client_interface.hpp

* sflib第一版本发布
* 版本号1.0.0
* 发布日期：2018-10-22
*/

#pragma once

#include "sf_tcp_client_interface.h"
#include "sf_net_utils.hpp"

namespace skyfire
{
    inline bool sf_tcp_client_interface::get_peer_addr(sf_addr_info_t &addr)
    {
        return skyfire::get_peer_addr(get_raw_socket(),addr);
    }

    inline bool sf_tcp_client_interface::get_local_addr(sf_addr_info_t &addr)
    {
        return skyfire::get_local_addr(get_raw_socket(),addr);
    }
}