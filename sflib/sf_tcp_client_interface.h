
/**
* @version 1.0.0
* @author skyfire
* @file sf_tcp_client_interface.h
*/

#pragma once

#include "sf_error.h"
#include "sf_net_utils.h"
#include "sf_nocopy.h"
#include "sf_object.h"
#include "sf_tcp_utils.h"
#include "sf_type.h"
#include <string>

namespace skyfire
{
/**
 *  @brief tcp客户端接口
 */
class tcp_client_interface : public nocopy<object>
{
    /**
     * 数据到来信号
     */
    sf_singal(data_coming, const pkg_header_t&, const byte_array&)
        /**
     * 原始数据到来信号
     */
        sf_singal(raw_data_coming, const byte_array&)
        /**
     * 关闭信号
     */
        sf_singal(closed)

        /**
     * 写入成功
     */
        sf_singal(write_finished)

        /**
     * 写入出错
     */
        sf_singal(write_error)

            private : public :
        /**
     * 获取原始socket
     * @return 原始socket
     */
        virtual SOCKET raw_socket()
        = 0;

    /**
     * 绑定
     * @param ip ip地址
     * @param port 端口号
     * @return 绑定是否成功
     */
    virtual bool bind(const std::string& ip, unsigned short port) = 0;

    /**
     * 连接服务器
     * @param ip 服务器ip
     * @param port 服务器端口
     * @return 是否连接成功
     */
    virtual sf_error connect_to_server(const std::string& ip,
                                   unsigned short     port)
        = 0;

    /**
     * 发送数据
     * @param type 数据类型
     * @param data 数据
     * @return 是否发送成功
     */
    virtual bool send(int type, const byte_array& data) = 0;

    /**
     * 发送数据
     * @param data 数据
     * @return 是否发送成功
     */
    virtual bool send(const byte_array& data) = 0;

    /**
     * 关闭
     */
    virtual void close() = 0;

    /**
     * 获取远端ip地址
     * @param addr 远端ip地址
     * @return 是否成功
     */
    bool peer_addr(addr_info_t& addr);

    /**
     * 获取本地ip地址
     * @param addr 本地ip地址
     * @return 是否成功
     */
    bool local_addr(addr_info_t& addr);
};

} // namespace skyfire