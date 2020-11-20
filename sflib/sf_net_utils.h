
/**
* @version 1.0.0
* @author skyfire
* @file sf_net_utils.h
*/

#pragma once

#include "sf_json.hpp"

#ifdef _WIN32
#include <winsock2.h>
#else

#endif

namespace skyfire
{

/**
     * @brief  地址信息
     */
struct addr_info_t
{
    std::string    ip;   // ip
    unsigned short port; // 端口
};
SF_JSONIFY(addr_info_t, ip, port)

/**
     * 获取远端地址
     * @param sock socket
     * @param addr 远端地址信息
     * @return 是否获取成功
     */
inline bool peer_addr(SOCKET sock, addr_info_t& addr);

/**
     * 获取本地地址
     * @param sock socket
     * @param addr 本地地址信息
     * @return 是否获取成功
     */
inline bool local_addr(SOCKET sock, addr_info_t& addr);

}