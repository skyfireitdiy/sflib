#pragma once

namespace skyfire
{
    inline bool sf_tcp_server_interface::get_server_addr(addr_info_t &addr)
    {
        return get_local_addr(get_raw_socket(),addr);
    }
}