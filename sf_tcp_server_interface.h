
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

#include "sf_nocopy.h"
#include "sf_object.h"
#include "sf_type.h"
#include "sf_tcp_utils.h"
#include "sf_net_utils.h"
#include <string>

namespace skyfire
{
    /**
     *  @brief tcp服务器
     */
    class sf_tcp_server_interface : public sf_nocopy<sf_object>
    {
        /**
         * 新连接到来信号
         */
        SF_REG_SIGNAL(new_connection, SOCKET);
        /**
         * 数据包到来信号
         */
        SF_REG_SIGNAL(data_coming, SOCKET, const sf_pkg_header_t&, const byte_array&);
        /**
         * 原始数据到来信号
         */
        SF_REG_SIGNAL(raw_data_coming, SOCKET, const byte_array&);
        /**
         * 关闭信号
         */
        SF_REG_SIGNAL(closed, SOCKET);
    public:
        /**
         * 获取原始socket
         * @return 原始socket
         */
        virtual SOCKET get_raw_socket() = 0;
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
        bool get_server_addr(sf_addr_info_t &addr);
    };
}