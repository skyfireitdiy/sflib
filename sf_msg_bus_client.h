
/**
* @version 1.0.0
* @author skyfire
* @mail skyfireitdiy@hotmail.com
* @see http://github.com/skyfireitdiy/sflib
* @file sf_msg_bus_client.h

* sflib第一版本发布
* 版本号1.0.0
* 发布日期：2018-10-22
*/

/*
 * sf_msg_bus_client 消息总线客户端
 */

#pragma once

#include "sf_object.hpp"
#include "sf_type.hpp"
#include "sf_tcp_client.hpp"
#include "sf_msg_bus_utils.h"


namespace skyfire
{
    /**
     *  @brief 消息总线客户端
     */
    class sf_msg_bus_client : public sf_nocopy<sf_object>
    {
        /**
         * @brief msg_come 消息到来信号
         */
        SF_REG_SIGNAL(msg_come, std::string, byte_array);

    public:
        /**
         * @brief make_client 创建客户端
         * @return 客户端对象
         */
        static std::shared_ptr<sf_msg_bus_client> make_client();

        /**
         * @brief sf_msg_bus_client 构造函数
         */
        sf_msg_bus_client();

        /**
         * 析构函数
         */
        ~sf_msg_bus_client();

        /**
         * @brief reg_msg_to_bus 向消息总线注册消息
         * @param type 消息类型
         */
        void reg_msg_to_bus(const std::string& type);

        /**
         * @brief reg_msg_to_bus 向消息总线批量注册消息
         * @param types 消息类型集合
         */
        void reg_msg_to_bus(const std::vector<std::string> &types);
        /**
         * @brief unreg_msg_to_bus 向消息总线反注册消息
         * @param type 消息类型
         */
        void unreg_msg_to_bus(const std::string& type);

        /**
         * @brief unreg_msg_to_bus 向消息总线批量反注册消息
         * @param types 消息类型集合
         */
        void unreg_msg_to_bus(const std::vector<std::string> &types);

        /**
         * @brief connect_to_server 连接消息总线
         * @param ip ip
         * @param port 端口
         * @return 连接结果
         */
        bool connect_to_server(const std::string &ip, unsigned short port);

        /**
         * @brief send_msg 向消息总线发送消息
         * @param type 类型
         * @param data 内容
         */
        void send_msg(const std::string& type, const byte_array& data);

        /**
         * @brief close 关闭总线客户端
         */
        void close();

        /**
         * 获取本地地址
         * @param addr 地址
         * @return 是否获取成功
         */
        bool get_local_addr(sf_addr_info_t& addr);

        /**
         * 获取远端地址
         * @param addr 地址信息
         * @return 是否获取成功
         */
        bool get_peer_addr(sf_addr_info_t& addr);

    private:
        std::shared_ptr<sf_tcp_client> p_client__ = sf_tcp_client::make_client();

        void on_reg_data__(const sf_pkg_header_t& header, const byte_array& data);

    };

}

