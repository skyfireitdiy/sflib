#pragma once

#include "sf_tcp_nat_traversal_server.h"

namespace skyfire {


    inline void sf_tcp_nat_traversal_server::on_new_connnection__(SOCKET sock) {
        // TODO 做一些处理
        sf_debug("新连接到来", sock);
    }

    inline void sf_tcp_nat_traversal_server::close() {
        server__->close();
        clients__.clear();
    }

    inline bool sf_tcp_nat_traversal_server::listen(const std::string &ip, unsigned short port) {
        if (running__)
            return false;
        running__ = true;
        return server__->listen(ip, port);
    }

    inline sf_tcp_nat_traversal_server::sf_tcp_nat_traversal_server() {
        sf_bind_signal(server__, new_connection, [=](SOCKET sock) {
            on_new_connnection__(sock);
        }, true);
        sf_bind_signal(server__, closed, [=](SOCKET sock) {
            on_disconnect__(sock);
        }, true);

        sf_bind_signal(server__, data_coming, [=](SOCKET sock, const pkg_header_t &header, const byte_array &data) {
            on_msg_coming__(sock, header, data);
        }, true);
    }

    inline std::shared_ptr <sf_tcp_nat_traversal_server> sf_tcp_nat_traversal_server::make_server() {
        return std::make_shared<sf_tcp_nat_traversal_server>();
    }

    inline void
    sf_tcp_nat_traversal_server::on_client_require_connect_to_peer_client__(sf_tcp_nat_traversal_context_t__ &context) {
        // 第1步：server获取发起端信息，提交至接受端，context有效字段：connect_id、dest_id、src_id、src_addr
        context.step = 1;
        // 判断来源是否存在
        if (clients__.count(context.src_id) == 0) {
            return;
        }
        // 判断目的是否存在，如果不存在，通知来源
        if (clients__.count(context.dest_id) == 0) {
            context.error_code = SF_ERR_NOT_EXIST;
            server__->send(context.src_id, TYPE_NAT_TRAVERSAL_ERROR, sf_serialize_binary(context));
            return;
        }
        // 获取来源的外网IP和端口，填充到连接上下文
        if (!get_peer_addr(context.src_id, context.src_addr)) {
            context.error_code = SF_ERR_DISCONNECT;
            server__->send(context.src_id, TYPE_NAT_TRAVERSAL_ERROR, sf_serialize_binary(context));
            return;
        }
        sf_debug("通知目的地址");
        // 将来源的外网ip端口通知给目的
        if (!server__->send(context.dest_id, TYPE_NAT_TRAVERSAL_NEW_CONNECT_REQUIRED, sf_serialize_binary(context))) {
            context.error_code = SF_ERR_REMOTE_ERR;
            server__->send(context.src_id, TYPE_NAT_TRAVERSAL_ERROR, sf_serialize_binary(context));
            return;
        }
    }

    inline void sf_tcp_nat_traversal_server::on_msg_coming__(SOCKET sock, const pkg_header_t &header, const byte_array &data) {
        sf_debug(header.type);
        switch (header.type) {
            case TYPE_NAT_TRAVERSAL_GET_LIST:
                on_update_client_list__(sock);
                break;
            case TYPE_NAT_TRAVERSAL_REG:
                on_client_reg__(sock);
                break;
            case TYPE_NAT_TRAVERSAL_REQUIRE_CONNECT_PEER: {
                sf_debug("收到连接请求");
                sf_tcp_nat_traversal_context_t__ context;
                sf_deserialize_binary(data, context, 0);
                on_client_require_connect_to_peer_client__(context);
            }
                break;
            case TYPE_NAT_TRAVERSAL_B_REPLY_ADDR: {
                sf_debug("收到B的回复");
                sf_tcp_nat_traversal_context_t__ context;
                sf_deserialize_binary(data, context, 0);
                on_nat_traversal_b_reply_addr(context, sock);
            }
                break;
            default:
                break;
        }
    }

    inline void
    sf_tcp_nat_traversal_server::on_nat_traversal_b_reply_addr(sf_tcp_nat_traversal_context_t__ &context, SOCKET sock) {
        // 第4步，服务器将目的的监听地址信息发送至来源，context有效字段：connect_id、dest_id、src_id、src_addr、dest_addr
        context.step = 4;
        // 判断目的是否存在，不存在，通知来源
        if (clients__.count(context.dest_id) == 0) {
            context.error_code = SF_ERR_NOT_EXIST;
            server__->send(context.src_id, TYPE_NAT_TRAVERSAL_ERROR, sf_serialize_binary(context));
            return;
        }
        // 判断来源是否存在，如果不存在，通知回复者
        if (clients__.count(context.src_id) == 0) {
            context.error_code = SF_ERR_NOT_EXIST;
            server__->send(context.dest_id, TYPE_NAT_TRAVERSAL_ERROR, sf_serialize_binary(context));
            return;
        }

        if (!get_peer_addr(sock, context.dest_addr)) {
            context.error_code = SF_ERR_DISCONNECT;
            server__->send(context.dest_id, TYPE_NAT_TRAVERSAL_ERROR, sf_serialize_binary(context));
            return;
        }
        sf_debug("给A回复B的地址");
        if (!server__->send(context.src_id, TYPE_NAT_TRAVERSAL_SERVER_REPLY_B_ADDR, sf_serialize_binary(context))) {
            context.error_code = SF_ERR_REMOTE_ERR;
            server__->send(context.dest_id, TYPE_NAT_TRAVERSAL_ERROR, sf_serialize_binary(context));
            return;
        }
    }

    inline void sf_tcp_nat_traversal_server::on_update_client_list__(int sock) {
        std::set<unsigned long long> tmp_data{clients__.begin(), clients__.end()};
        auto data = sf_serialize_binary(tmp_data);
        if (sock == -1) {
            for (auto &p:clients__) {
                server__->send(p, TYPE_NAT_TRAVERSAL_LIST, data);
            }
        } else {
            server__->send(sock, TYPE_NAT_TRAVERSAL_LIST, data);
        }
    }

    inline void sf_tcp_nat_traversal_server::on_client_reg__(SOCKET sock) {
        clients__.insert(sock);
        unsigned long long id = sock;
        server__->send(sock, TYPE_NAT_TRAVERSAL_SET_ID, sf_serialize_binary(id));
        on_update_client_list__();
    }

    inline void sf_tcp_nat_traversal_server::on_disconnect__(SOCKET sock) {
        clients__.erase(sock);
        on_update_client_list__();
    }
}