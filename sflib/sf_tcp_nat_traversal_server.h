
/**
* @version 1.0.0
* @author skyfire
* @file sf_tcp_nat_traversal_server.h
*/

#pragma once

#include "sf_object.hpp"
#include "sf_stdc++.h"
#include "sf_tcp_client.h"
#include "sf_tcp_nat_traversal_utils.hpp"
#include "sf_tcp_server.h"
#include "sf_nocopy.h"
#include "sf_utils.h"

namespace skyfire {
/**
 * nat穿透服务器类
 */
class tcp_nat_traversal_server final
    : public make_instance_t<tcp_nat_traversal_server,
          nocopy<object>> {
private:
    // 保存客户端列表
    std::unordered_set<SOCKET> clients__;
    // Server
    std::shared_ptr<tcp_server> server__ { tcp_server::make_instance() };
    // 当前已运行
    bool running__ = false;

    static void on_new_connection__(SOCKET sock);

    void on_disconnect__(SOCKET sock);

    void on_client_reg__(SOCKET sock);

    void on_update_client_list__(SOCKET sock = -1);

    void on_nat_traversal_b_reply_addr(
        tcp_nat_traversal_context_t__& context, SOCKET sock) const;

    void on_msg_coming__(SOCKET sock, const pkg_header_t& header,
        const byte_array& data);

    void on_client_require_connect_to_peer_client__(
        tcp_nat_traversal_context_t__& context) const;

public:
    /**
     * 构造函数
     */
    tcp_nat_traversal_server();

    /**
     * 监听端口
     * @param ip 本机IP
     * @param port 本机端口
     * @return 监听结果
     */
    bool listen(const std::string& ip, unsigned short port);

    /**
     * 关闭
     */
    void close();
};

} // namespace skyfire