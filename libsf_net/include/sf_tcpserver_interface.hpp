
/**
* @version 1.0.0
* @author skyfire
* @email skyfireitdiy@hotmail.com
* @see http://github.com/skyfireitdiy/sflib
* @file sf_tcpserver_interface.hpp

* sflib第一版本发布
* 版本号1.0.0
* 发布日期：2018-10-22
*/

#pragma once

namespace skyfire
{
    inline bool sf_tcpserver_interface::get_server_addr(addr_info_t &addr)
    {
        return get_local_addr(get_raw_socket(),addr);
    }
}