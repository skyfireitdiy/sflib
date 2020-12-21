
/**
* @version 1.0.0
* @author skyfire
* @file sf_net_utils.h
*/

#pragma once

#include "sf_data_buffer.h"
#include "sf_error.h"
#include "sf_json.hpp"
#include "sf_multi_value.h"
#include "sf_object.h"
#include "sf_type.h"
#include "sf_utils.h"
#include <memory>

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

using ip_list_result = multi_value<sf_error, std::vector<std::string>>;

/**
 * @brief 解析dns
 * 
 * @param host 域名/主机名
 * @return ip_list_result ip地址列表
 */

inline ip_list_result resolve_dns(const std::string& host);
}