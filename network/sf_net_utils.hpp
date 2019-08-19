
/**
* @version 1.0.0
* @author skyfire
* @mail skyfireitdiy@hotmail.com
* @see http://github.com/skyfireitdiy/sflib
* @file sf_net_utils.hpp

* sflib第一版本发布
* 版本号1.0.0
* 发布日期：2018-10-22
*/

#pragma once

#ifdef _WIN32
#include <winsock2.h>
#include <WS2tcpip.h>
#else
#include <unistd.h>
#include <sys/types.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <sys/resource.h>
#endif

#include "sf_net_utils.h"

namespace skyfire
{


    inline bool peer_addr(SOCKET sock, sf_addr_info_t &addr) {
#ifdef _WIN32
        SOCKADDR_IN sock_addr;
        memset(&sock_addr,0, sizeof(sock_addr));
        int len = sizeof(sock_addr);
        if(getpeername(sock, reinterpret_cast<SOCKADDR*>(&sock_addr), &len) != 0){
            return false;
        }
		
    	sf_safe_inet_ntoa(AF_INET, addr.ip, sock_addr.sin_addr);

        addr.port = ntohs(sock_addr.sin_port);
        return true;
#else
        sockaddr_in sock_addr{};
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

    inline bool local_addr(SOCKET sock, sf_addr_info_t &addr) {
#ifdef _WIN32
        SOCKADDR_IN sock_addr;
        memset(&sock_addr,0, sizeof(sock_addr));
        int len = sizeof(sock_addr);
        if(getsockname(sock, reinterpret_cast<SOCKADDR*>(&sock_addr), &len) != 0){
            return false;
        }
		sf_safe_inet_ntoa(AF_INET, addr.ip, sock_addr.sin_addr);
        addr.port = ntohs(sock_addr.sin_port);
        return true;
#else
        sockaddr_in sock_addr{};
        memset(&sock_addr,0, sizeof(sock_addr));
        socklen_t len = sizeof(sock_addr);
        if(getsockname(sock, reinterpret_cast<sockaddr*>(&sock_addr), &len) != 0){
            return false;
        }
		sf_safe_inet_ntoa(AF_INET, addr.ip, sock_addr.sin_addr);
        addr.port = ntohs(sock_addr.sin_port);
        return true;
#endif
    }

}