#pragma once

#include "sf_serialize_binary.hpp"

#ifdef _WIN32
#include <winsock2.h>
#else

#endif

namespace skyfire
{

    // 地址信息
    struct addr_info_t{
        std::string ip;
        unsigned short port;
    };
    SF_MAKE_SERIALIZABLE_BINARY(addr_info_t,ip,port)

    inline bool get_peer_addr(SOCKET sock, addr_info_t &addr);

    inline bool get_local_addr(SOCKET sock, addr_info_t &addr);

}