#pragma once

#include "sf_serialize_binary.h"

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

    inline bool get_peer_addr(SOCKET sock, addr_info_t &addr){
#ifdef _WIN32
        SOCKADDR_IN sock_addr;
        memset(&sock_addr,0, sizeof(sock_addr));
        int len = sizeof(sock_addr);
        if(getpeername(sock, reinterpret_cast<SOCKADDR*>(&sock_addr), &len) != 0){
            return false;
        }
        addr.ip = inet_ntoa(sock_addr.sin_addr);
        addr.port = ntohs(sock_addr.sin_port);
        return true;
#else
        sockaddr_in sock_addr;
        memset(&sock_addr,0, sizeof(sock_addr));
        socklen_t len = sizeof(sock_addr);
        if(getpeername(sock, reinterpret_cast<sockaddr*>(&sock_addr), &len) != 0){
            return false;
        }
        addr.ip = inet_ntoa(sock_addr.sin_addr);
        addr.port = ntohs(sock_addr.sin_port);
        return true;
#endif
    }


    inline bool get_local_addr(SOCKET sock, addr_info_t &addr){
#ifdef _WIN32
        SOCKADDR_IN sock_addr;
        memset(&sock_addr,0, sizeof(sock_addr));
        int len = sizeof(sock_addr);
        if(getsockname(sock, reinterpret_cast<SOCKADDR*>(&sock_addr), &len) != 0){
            return false;
        }
        addr.ip = inet_ntoa(sock_addr.sin_addr);
        addr.port = ntohs(sock_addr.sin_port);
        return true;
#else
        sockaddr_in sock_addr;
        memset(&sock_addr,0, sizeof(sock_addr));
        socklen_t len = sizeof(sock_addr);
        if(getsockname(sock, reinterpret_cast<sockaddr*>(&sock_addr), &len) != 0){
            return false;
        }
        addr.ip = inet_ntoa(sock_addr.sin_addr);
        addr.port = ntohs(sock_addr.sin_port);
        return true;
#endif
    }



}