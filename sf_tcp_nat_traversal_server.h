
/**
* @version 1.0.0
* @author skyfire
* @mail skyfireitdiy@hotmail.com
* @see http://github.com/skyfireitdiy/sflib
* @file sf_tcp_nat_traversal_server.h

* sflib第一版本发布
* 版本号1.0.0
* 发布日期：2018-10-22
*/

#pragma once

#include <set>
#include "sf_nocopy.h"
#include "sf_object.hpp"
#include "sf_tcp_client.hpp"
#include "sf_tcp_server.hpp"
#include "sf_define.h"
#include "sf_tcp_nat_traversal_utils.hpp"

namespace skyfire {
    /**
     * nat穿透服务器类
     */
    class sf_tcp_nat_traversal_server : public sf_nocopy<sf_object> {
    private:
        // 保存客户端列表
        std::set<SOCKET> clients__;
        // Server
        std::shared_ptr<sf_tcp_server> server__{sf_tcp_server::make_server()};
        // 当前已运行
        bool running__ = false;

        void on_new_connnection__(SOCKET sock);

        void on_disconnect__(SOCKET sock);

        void on_client_reg__(SOCKET sock);

        void on_update_client_list__(SOCKET sock = static_cast<SOCKET>(-1));

        void on_nat_traversal_b_reply_addr(sf_tcp_nat_traversal_context_t__ &context, SOCKET sock);

        void on_msg_coming__(SOCKET sock, const sf_pkg_header_t &header, const byte_array &data);

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