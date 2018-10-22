
/**
* @version 1.0.0
* @author skyfire
* @mail skyfireitdiy@hotmail.com
* @see http://github.com/skyfireitdiy/sflib
* @file sf_tcp_server_interface.hpp

* sflib第一版本发布
* 版本号1.0.0
* 发布日期：2018-10-22
*/

#pragma once

namespace skyfire
{
    inline bool sf_tcp_server_interface::get_server_addr(sf_addr_info_t &addr)
    {
        return get_local_addr(get_raw_socket(),addr);
    }
}