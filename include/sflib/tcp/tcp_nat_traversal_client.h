
#pragma once
#include "sflib/object/object.h"
#include "sflib/tcp/tcp_client.h"
#include "sflib/tcp/tcp_nat_traversal_utils.h"
#include "sflib/tcp/tcp_server.h"
#include "sflib/tools/nocopy.h"
#include "sflib/tools/utils.h"

#include <zconf.h>

namespace skyfire
{
struct p2p_connect_context_t__
{
    tcp_nat_traversal_context_t__ tcp_nat_traversal_context;
    std::shared_ptr<tcp_client> point_a_client_1;
    std::shared_ptr<tcp_client> point_a_client_2;
    std::shared_ptr<tcp_server> point_a_server;
    std::shared_ptr<tcp_client> point_b_client_1;
    std::shared_ptr<tcp_client> point_b_client_2;
    std::shared_ptr<tcp_server> point_b_server;
};
class tcp_nat_traversal_client
    : public make_instance_t<tcp_nat_traversal_client,
                             nocopy<object> >
{
    // 新连接，返回建立连接成功的tcpserver和已经连接上的socket，可通过这两个与被连接方通信
    sf_singal(new_nat_connection,
              std::shared_ptr<tcp_nat_traversal_connection>, std::string) private : std::shared_ptr<tcp_client> client__ { tcp_client::make_instance() };
    std::unordered_set<unsigned long long> client_list__;
    unsigned long long self_id__;
    std::unordered_map<std::string, p2p_connect_context_t__>
        connect_context_map__;
    addr_info_t server_addr__;
    void on_new_connect_required__(tcp_nat_traversal_context_t__ &context);
    void on_server_reply_b_addr(tcp_nat_traversal_context_t__ &context);
    void on_client_data_coming__(const pkg_header_t &header,
                                 const byte_array &data);

public:
    tcp_nat_traversal_client();
    unsigned long long int id() const;
    bool connect_to_server(const std::string &ip, unsigned short port);
    std::unordered_set<unsigned long long> clients() const;
    std::string connect_to_peer(unsigned long long peer_id, bool raw);
    void close() const;
};
} // namespace skyfire