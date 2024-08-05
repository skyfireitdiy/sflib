
#pragma once
#include "sflib/object/object.h"
#include "sflib/tcp/tcp_client.h"
#include "sflib/tcp/tcp_nat_traversal_utils.h"
#include "sflib/tcp/tcp_server.h"
#include "sflib/tools/nocopy.h"
#include "sflib/tools/utils.h"

namespace skyfire
{
class tcp_nat_traversal_server final
    : public make_instance_t<tcp_nat_traversal_server,
                             nocopy<object> >
{
private:
    // 保存客户端列表
    std::unordered_set<SOCKET> clients__;
    // Server
    std::shared_ptr<tcp_server> server__ {tcp_server::make_instance()};
    // 当前已运行
    bool running__ = false;
    static void on_new_connection__(SOCKET sock);
    void on_disconnect__(SOCKET sock);
    void on_client_reg__(SOCKET sock);
    void on_update_client_list__(SOCKET sock = -1);
    void on_nat_traversal_b_reply_addr(
        tcp_nat_traversal_context_t__ &context, SOCKET sock) const;
    void on_msg_coming__(SOCKET sock, const pkg_header_t &header,
                         const byte_array &data);
    void on_client_require_connect_to_peer_client__(
        tcp_nat_traversal_context_t__ &context) const;

public:
    tcp_nat_traversal_server();
    bool listen(const std::string &ip, unsigned short port);
    void close();
};
} // namespace skyfire