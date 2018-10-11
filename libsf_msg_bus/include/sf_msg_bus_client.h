/*
 * sf_msg_bus_client 消息总线客户端
 */

#pragma once

#include "sf_object.hpp"
#include "sf_type.hpp"
#include "sf_tcpclient.hpp"
#include "sf_msg_bus_utils.hpp"


namespace skyfire
{
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
         * @brief connect_to_bus 连接消息总线
         * @param ip ip
         * @param port 端口
         * @return 连接结果
         */
        bool connect_to_bus(const std::string &ip, unsigned short port);

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

        bool get_local_addr(addr_info_t& addr);

        bool get_peer_addr(addr_info_t& addr);

    private:
        std::shared_ptr<sf_tcpclient> p_client__ = sf_tcpclient::make_client();

        void on_reg_data__(const pkg_header_t& header, const byte_array& data);

    };

}

