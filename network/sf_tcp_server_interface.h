
/**
* @version 1.0.0
* @author skyfire
* @mail skyfireitdiy@hotmail.com
* @see http://github.com/skyfireitdiy/sflib
* @file sf_tcp_server_interface.h

* sflib第一版本发布
* 版本号1.0.0
* 发布日期：2018-10-22
*/

#pragma once

#include "core/sf_object.h"
#include "core/sf_stdc++.h"
#include "core/sf_type.h"
#include "network/sf_net_utils.h"
#include "network/sf_server_socket_filter.h"
#include "sf_tcp_utils.h"
#include "tools/sf_nocopy.h"

namespace skyfire {
/**
 *  @brief tcp服务器
 */
class sf_tcp_server_interface : public sf_nocopy<sf_object> {
    /**
     * 新连接到来信号
     */
    SF_REG_SIGNAL(new_connection, SOCKET);
    /**
     * 数据包到来信号
     */
    SF_REG_SIGNAL(data_coming, SOCKET, const sf_pkg_header_t &,
                  const byte_array &);
    /**
     * 原始数据到来信号
     */
    SF_REG_SIGNAL(raw_data_coming, SOCKET, const byte_array &);
    /**
     * 关闭信号
     */
    SF_REG_SIGNAL(closed, SOCKET);

    /**
     * 写成功信号
     */
    SF_REG_SIGNAL(write_finished, SOCKET);

    /**
     * 写失败信号
     */
    SF_REG_SIGNAL(write_error, SOCKET);

   private:
    std::vector<std::shared_ptr<sf_server_socket_filter>> filters__;

   protected:
    bool manage_clients__{true};

    void after_raw_recv_filter__(SOCKET sock, byte_array &data);
    void after_recv_filter__(SOCKET sock, sf_pkg_header_t &header,
                             byte_array &data);
    void before_raw_send_filter__(SOCKET sock, byte_array &data);
    void before_send_filter__(SOCKET sock, sf_pkg_header_t &header,
                              byte_array &data);
    void new_connection_filter__(SOCKET sock);
    void listen_sock_filter__(SOCKET sock);
    void disconnect_sock_filter__(SOCKET sock);

   public:
#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCUnusedGlobalDeclarationInspection"
    void add_server_socket_filter(
        std::shared_ptr<sf_server_socket_filter> &&filter);
#pragma clang diagnostic pop

    /**
     * 获取原始socket
     * @return 原始socket
     */
    virtual SOCKET raw_socket() = 0;
    /**
     * 监听端口
     * @param ip 本地ip
     * @param port 本地端口
     * @return 是否监听成功
     */
    virtual bool listen(const std::string &ip, unsigned short port) = 0;

    /**
     * 关闭服务器
     */
    virtual void close() = 0;
    /**
     * 关闭指定连接
     * @param sock 指定连接socket
     */
    virtual void close(SOCKET sock) = 0;

    /**
     * 发送数据包
     * @param sock socket
     * @param type 包类型
     * @param data 包数据
     * @return 是否发送成功
     */
    virtual bool send(SOCKET sock, int type, const byte_array &data) = 0;

    /**
     * 发送数据
     * @param sock scoket
     * @param data 数据
     * @return 是否发送成功
     */
    virtual bool send(SOCKET sock, const byte_array &data) = 0;

    /**
     * 获取server地址
     * @param addr server地址信息
     * @return 获取结果
     */
    bool server_addr(sf_addr_info_t &addr);

#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCUnusedGlobalDeclarationInspection"
    /**
     * 设置是否管理客户端连接
     * @param flag 是否管理
     */
    void set_manage_clients(bool flag);
#pragma clang diagnostic pop
};
}    // namespace skyfire