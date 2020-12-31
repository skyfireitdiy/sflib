
/**
* @version 1.0.0
* @author skyfire
* @file sf_tcp_nat_traversal_client.h
*/

#pragma once

#include "sf_nocopy.h"
#include "sf_object.hpp"
#include "sf_stdc++.h"
#include "sf_tcp_client.h"
#include "sf_tcp_nat_traversal_utils.hpp"
#include "sf_tcp_server.h"
#include "sf_utils.h"
#include <zconf.h>

namespace skyfire
{

/**
 * @brief p2p客户端连接上下文
 */
struct p2p_connect_context_t__
{
    tcp_nat_traversal_context_t__ tcp_nat_traversal_context;
    std::shared_ptr<tcp_client>   point_a_client_1;
    std::shared_ptr<tcp_client>   point_a_client_2;
    std::shared_ptr<tcp_server>   point_a_server;
    std::shared_ptr<tcp_client>   point_b_client_1;
    std::shared_ptr<tcp_client>   point_b_client_2;
    std::shared_ptr<tcp_server>   point_b_server;
};

/**
 *  @brief nat穿透客户端类
 */
class tcp_nat_traversal_client
    : public make_instance_t<tcp_nat_traversal_client,
                             nocopy<object>>
{
    // 新连接，返回建立连接成功的tcpserver和已经连接上的socket，可通过这两个与被连接方通信
    sf_singal(new_nat_connection,
              std::shared_ptr<tcp_nat_traversal_connection>, std::string)

        private : std::shared_ptr<tcp_client> client__ { tcp_client::make_instance() };
    std::unordered_set<unsigned long long> client_list__;
    unsigned long long                     self_id__;
    std::unordered_map<std::string, p2p_connect_context_t__>
                connect_context_map__;
    addr_info_t server_addr__;

    void on_new_connect_required__(tcp_nat_traversal_context_t__& context);

    void on_server_reply_b_addr(tcp_nat_traversal_context_t__& context);

    void on_client_data_coming__(const pkg_header_t& header,
                                 const byte_array&   data);

public:
    tcp_nat_traversal_client();

    /**
     * 获取id
     * @return id
     */
    unsigned long long int id() const;

    /**
     * 连接到穿透服务器，连接成功后会注册
     * @param ip 穿透服务器ip
     * @param port 穿透服务器端口
     * @return 连接结果
     */
    bool connect_to_server(const std::string& ip, unsigned short port);

    /**
     * 获取终端集合
     * @return 终端集合
     */
    std::unordered_set<unsigned long long> clients() const;

    /**
     * 来源，连接远程peer端
     * @param peer_id 远端id
     * @param raw 是否为原始数据格式
     * @return 空表示失败，其他表示此次连接的id
     */
    std::string connect_to_peer(unsigned long long peer_id, bool raw);

    /**
     * 关闭
     */
    void close() const;
};

} // namespace skyfire