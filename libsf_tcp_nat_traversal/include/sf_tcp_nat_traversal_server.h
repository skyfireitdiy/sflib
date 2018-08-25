#pragma once

#include <set>
#include "sf_nocopy.hpp"
#include "sf_object.hpp"
#include "sf_tcpclient.hpp"
#include "sf_tcpserver.hpp"
#include "sf_define.hpp"
#include "sf_tcp_nat_traversal_utils.hpp"

namespace skyfire {
    class sf_tcp_nat_traversal_server : public sf_nocopy<sf_object> {
    private:
        // 保存客户端列表
        std::set<SOCKET> clients__;

        // Server
        std::shared_ptr<sf_tcpserver> server__{sf_tcpserver::make_server()};


        // 当前已运行
        bool running__ = false;

        /**
         * 主Server新连接到来处理
         * @param sock 新连接的socket
         */
        void on_new_connnection__(SOCKET sock);

        /**
         * 主Server连接断开处理
         * @param sock 断开的socket
         */
        void on_disconnect__(SOCKET sock);

        /**
         * 主Server处理注册请求
         * @param sock 注册的Socket
         */
        void on_client_reg__(SOCKET sock);

        /**
         * 更新客户信息
         * @param sock
         */
        void on_update_client_list__(SOCKET sock = static_cast<SOCKET>(-1));

        void on_nat_traversal_b_reply_addr(sf_tcp_nat_traversal_context_t__ &context, SOCKET sock);

        /**
         * 消息到来处理
         * @param sock 来源socket
         * @param header 消息头
         * @param data 数据
         */
        void on_msg_coming__(SOCKET sock, const pkg_header_t &header, const byte_array &data);

        /**
         * 处理客户端远程连接请求
         * @param context
         */
        void on_client_require_connect_to_peer_client__(sf_tcp_nat_traversal_context_t__ &context);

    public:
        /**
         * 创建NAT穿透服务器
         * @return 服务器smart指针
         */
        static std::shared_ptr<sf_tcp_nat_traversal_server> make_server();

        /**
         * 构造函数
         */
        sf_tcp_nat_traversal_server();

        /**
         * 监听端口
         * @param ip 本机IP
         * @param port 本机端口
         * @return 监听结果
         */
        bool listen(const std::string &ip, unsigned short port);

        /**
         * 关闭
         */
        void close();
    };

}