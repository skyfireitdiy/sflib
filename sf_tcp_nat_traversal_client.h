
/**
* @version 1.0.0
* @author skyfire
* @mail skyfireitdiy@hotmail.com
* @see http://github.com/skyfireitdiy/sflib
* @file sf_tcp_nat_traversal_client.h

* sflib第一版本发布
* 版本号1.0.0
* 发布日期：2018-10-22
*/

#pragma once

#include "sf_nocopy.h"
#include "sf_object.hpp"
#include "sf_tcp_client.hpp"
#include "sf_tcp_server.hpp"
#include "sf_serialize_binary.hpp"
#include "sf_define.h"
#include "sf_tcp_nat_traversal_utils.hpp"
#include "sf_random.hpp"
#include "sf_stdc++.h"
#include <zconf.h>


namespace skyfire {

    /**
     * @brief p2p客户端连接上下文
     */
    struct sf_p2p_connect_context_t__ {
        sf_tcp_nat_traversal_context_t__ tcp_nat_traversal_context;
        std::shared_ptr<sf_tcp_client> point_a_client_1;
        std::shared_ptr<sf_tcp_client> point_a_client_2;
        std::shared_ptr<sf_tcp_server> point_a_server;
        std::shared_ptr<sf_tcp_client> point_b_client_1;
        std::shared_ptr<sf_tcp_client> point_b_client_2;
        std::shared_ptr<sf_tcp_server> point_b_server;
    };

    /**
     *  @brief nat穿透客户端类
     */
    class sf_tcp_nat_traversal_client : public sf_nocopy<sf_object> {

        // 新连接，返回建立连接成功的tcpserver和已经连接上的socket，可通过这两个与被连接方通信
    SF_REG_SIGNAL(new_nat_connection, std::shared_ptr<sf_tcp_nat_traversal_connection>, int)

    private:
        std::shared_ptr<sf_tcp_client> client__{sf_tcp_client::make_client()};
        std::set<unsigned long long> client_list__;
        unsigned long long self_id__;
        std::map<int, sf_p2p_connect_context_t__> connect_context_map__;
        sf_addr_info_t server_addr__;

        void on_new_connect_required__(sf_tcp_nat_traversal_context_t__ &context);

        void on_server_reply_b_addr(sf_tcp_nat_traversal_context_t__ &context);


        void on_client_data_coming__(const sf_pkg_header_t &header, const byte_array &data);

        void on_client_close__();

        sf_tcp_nat_traversal_client();
    public:

        /**
         * 生成客户端
         * @return
         */
        static std::shared_ptr<sf_tcp_nat_traversal_client> make_client();

        /**
         * 获取id
         * @return id
         */
        unsigned long long int get_id();

        /**
         * 连接到穿透服务器，连接成功后会注册
         * @param ip 穿透服务器ip
         * @param port 穿透服务器端口
         * @return 连接结果
         */
        bool connect_to_server(const std::string &ip, unsigned short port);

        /**
         * 获取终端集合
         * @return 终端集合
         */
        std::set<unsigned long long> get_clients();

        /**
         * 来源，连接远程peer端
         * @param peer_id 远端id
         * @param raw 是否为原始数据格式
         * @return -1表示失败，其他表示此次连接的id
         */
        int connect_to_peer(unsigned long long peer_id, bool raw);

        /**
         * 关闭
         */
        void close();
    };

}