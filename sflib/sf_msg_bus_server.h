
/**
* @version 1.0.0
* @author skyfire
* @file sf_msg_bus_server.h
*/

/*
 * msg_bus_server 消息总线服务器
 */
#pragma once

#include <string>

#include "sf_object.hpp"
#include "sf_tcp_server.h"
#include "sf_utils.h"

namespace skyfire
{

/**
 *  @brief 消息总线服务端
 */
class msg_bus_server final
    : public make_instance_t<msg_bus_server, nocopy<object>>
{
    /**
     * @brief msg_come 消息到来信号
     */
    sf_singal(msg_come, SOCKET, std::string, byte_array);

public:
    /**
     * @brief msg_bus_server 构造函数
     */
    msg_bus_server();
    /**
     * 析构函数
     */
    ~msg_bus_server() override;

    /**
     * @brief listen 监听消息总线
     * @param ip ip
     * @param port 端口
     * @return 监听是否成功
     */
    bool listen(const std::string& ip, unsigned short port) const;

    /**
     * @brief close 关闭总线服务器
     */
    void close();

    /**
     * @brief clear_client 清理客户端
     */
    void clear_client();

    /**
     * @brief send_msg 发送消息
     * @param type 类型
     * @param data 数据
     */
    void send_msg(const std::string& type, const byte_array& data);

    /**
     * 获取服务器地址
     * @param addr 地址信息
     * @return 是否成功
     */
    bool server_addr(addr_info_t& addr) const;

private:
    std::shared_ptr<tcp_server> p_server__ = tcp_server::make_instance();

    std::unordered_map<std::string, std::list<SOCKET>> msg_map__;

    void reg_msg__(SOCKET sock, const std::string& msg_name);

    void on_reg_data__(SOCKET sock, const pkg_header_t& header,
                       const byte_array& data);

    void unreg_msg__(SOCKET sock, const std::string& msg);

    void on_disconnect__(SOCKET sock);
};
} // namespace skyfire
