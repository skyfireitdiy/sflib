
/**
* @version 1.0.0
* @author skyfire
* @mail skyfireitdiy@hotmail.com
* @see http://github.com/skyfireitdiy/sflib
* @file sf_net_utils.h

* sflib第一版本发布
* 版本号1.0.0
* 发布日期：2018-10-22
*/

#pragma once

#include "tools/sf_json.hpp"

#ifdef _WIN32
#include <winsock2.h>
#else

#endif

namespace skyfire
{

    /**
     * @brief  地址信息
     */
    struct sf_addr_info_t{
        std::string ip;             // ip
        unsigned short port;      // 端口
    };
    SF_JSONIFY(sf_addr_info_t,ip,port)

    /**
     * 获取远端地址
     * @param sock socket
     * @param addr 远端地址信息
     * @return 是否获取成功
     */
    inline bool peer_addr(SOCKET sock, sf_addr_info_t &addr);

    /**
     * 获取本地地址
     * @param sock socket
     * @param addr 本地地址信息
     * @return 是否获取成功
     */
    inline bool local_addr(SOCKET sock, sf_addr_info_t &addr);

}