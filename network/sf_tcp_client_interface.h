
/**
* @version 1.0.0
* @author skyfire
* @mail skyfireitdiy@hotmail.com
* @see http://github.com/skyfireitdiy/sflib
* @file sf_tcp_client_interface.h

* sflib第一版本发布
* 版本号1.0.0
* 发布日期：2018-10-22
*/

#pragma once

#include <string>
#include "core/sf_object.h"
#include "core/sf_type.h"
#include "network/sf_net_utils.h"
#include "sf_tcp_utils.h"
#include "tools/sf_nocopy.h"

namespace skyfire {
/**
 *  @brief tcp客户端接口
 */
class sf_tcp_client_interface : public sf_nocopy<sf_object> {
    /**
     * 数据到来信号
     */
    SF_REG_SIGNAL(data_coming, const sf_pkg_header_t &, const byte_array &)
    /**
     * 原始数据到来信号
     */
    SF_REG_SIGNAL(raw_data_coming, const byte_array &)
    /**
     * 关闭信号
     */
    SF_REG_SIGNAL(closed)

    /**
     * 写入成功
     */
    SF_REG_SIGNAL(write_finished)

    /**
     * 写入出错
     */
    SF_REG_SIGNAL(write_error)

   private:
   public:
    /**
     * 获取原始socket
     * @return 原始socket
     */
    virtual SOCKET raw_socket() = 0;

    /**
     * 绑定
     * @param ip ip地址
     * @param port 端口号
     * @return 绑定是否成功
     */
    virtual bool bind(const std::string &ip, unsigned short port) = 0;

    /**
     * 连接服务器
     * @param ip 服务器ip
     * @param port 服务器端口
     * @return 是否连接成功
     */
    virtual bool connect_to_server(const std::string &ip,
                                   unsigned short port) = 0;

    /**
     * 发送数据
     * @param type 数据类型
     * @param data 数据
     * @return 是否发送成功
     */
    virtual bool send(int type, const byte_array &data) = 0;

    /**
     * 发送数据
     * @param data 数据
     * @return 是否发送成功
     */
    virtual bool send(const byte_array &data) = 0;

    /**
     * 关闭
     */
    virtual void close() = 0;

    /**
     * 获取远端ip地址
     * @param addr 远端ip地址
     * @return 是否成功
     */
    bool peer_addr(sf_addr_info_t &addr);

    /**
     * 获取本地ip地址
     * @param addr 本地ip地址
     * @return 是否成功
     */
    bool local_addr(sf_addr_info_t &addr);
};

}    // namespace skyfire