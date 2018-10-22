
/**
* @version 1.0.0
* @author skyfire
* @email skyfireitdiy@hotmail.com
* @see http://github.com/skyfireitdiy/sflib
* @file sf_tcpclient_interface.hpp

* sflib第一版本发布
* 版本号1.0.0
* 发布日期：2018-10-22
*/

#pragma once

#include "sf_tcpclient_interface.h"
#include "sf_netutils.hpp"

namespace skyfire
{
    inline bool sf_tcpclient_interface::get_peer_addr(addr_info_t &addr)
    {
        return skyfire::get_peer_addr(get_raw_socket(),addr);
    }

    inline bool sf_tcpclient_interface::get_local_addr(addr_info_t &addr)
    {
        return skyfire::get_local_addr(get_raw_socket(),addr);
    }
}