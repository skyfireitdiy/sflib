
/**
* @version 1.0.0
* @author skyfire
* @file sf_tcp_server_interface.h
*/

#pragma once

#include "sf_error.h"
#include "sf_net_utils.h"
#include "sf_nocopy.h"
#include "sf_object.h"
#include "sf_server_socket_filter.h"
#include "sf_stdc++.h"
#include "sf_tcp_utils.h"
#include "sf_type.h"

namespace skyfire
{
/**
 *  @brief tcp服务器
 */
class tcp_server_interface : public nocopy<object>
{
    /**
     * 新连接到来信号
     */
    sf_singal(new_connection, SOCKET);
    /**
     * 数据包到来信号
     */
    sf_singal(data_coming, SOCKET, const pkg_header_t&,
              const byte_array&);
    /**
     * 原始数据到来信号
     */
    sf_singal(raw_data_coming, SOCKET, const byte_array&);
    /**
     * 关闭信号
     */
    sf_singal(closed, SOCKET);

    /**
     * 写成功信号
     */
    sf_singal(write_finished, SOCKET);

    /**
     * 写失败信号
     */
    sf_singal(write_error, SOCKET);

    /*
     * 可读信号
     */
    sf_singal(ready_read, SOCKET);

    /*
     * 可写信号
     */
    sf_singal(ready_write, SOCKET);

private:
    std::vector<std::shared_ptr<server_socket_filter>> filters__;

protected:
    void after_raw_recv_filter__(SOCKET sock, byte_array& data);
    void after_recv_filter__(SOCKET sock, pkg_header_t& header,
                             byte_array& data);
    void before_raw_send_filter__(SOCKET sock, byte_array& data);
    void before_send_filter__(SOCKET sock, pkg_header_t& header,
                              byte_array& data);
    void new_connection_filter__(SOCKET sock);
    void listen_sock_filter__(SOCKET sock);
    void disconnect_sock_filter__(SOCKET sock);

public:
    void add_server_socket_filter(
        std::shared_ptr<server_socket_filter>&& filter);

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
    virtual bool listen(const std::string& ip, unsigned short port) = 0;

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
    virtual bool send(SOCKET sock, int type, const byte_array& data) = 0;

    /**
     * 发送数据
     * @param sock socket
     * @param data 数据
     * @return 是否发送成功
     */
    virtual bool send(SOCKET sock, const byte_array& data) = 0;

    /**
     * 获取server地址
     * @param addr server地址信息
     * @return 获取结果
     */
    bool server_addr(addr_info_t& addr);

    /**
     * @brief 读取套接字信息
     * 
     * @param sock 套接字
     * @param data 数据
     * @param length 长度
     * @return sf_error 错误信息
     */
    virtual sf_error recv(SOCKET sock, byte_array& data, int length = default_buffer_size) = 0;
};
} // namespace skyfire