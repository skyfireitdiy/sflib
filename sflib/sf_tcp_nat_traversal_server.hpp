
/**
* @version 1.0.0
* @author skyfire
* @file sf_tcp_nat_traversal_server.hpp
*/
#pragma once
#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCDFAInspection"

#include "sf_tcp_nat_traversal_server.h"
#include "sf_error.h"

namespace skyfire {

inline void tcp_nat_traversal_server::on_new_connection__(SOCKET sock) {
    // TODO 做一些处理
    sf_debug("new connection", sock);
}

inline void tcp_nat_traversal_server::close() {
    server__->close();
    clients__.clear();
}

inline bool tcp_nat_traversal_server::listen(const std::string &ip,
                                                unsigned short port) {
    if (running__) return false;
    running__ = true;
    return server__->listen(ip, port);
}

inline tcp_nat_traversal_server::tcp_nat_traversal_server() {
    sf_bind(
        server__, new_connection,
        [=](SOCKET sock) { on_new_connection__(sock); }, true);
    sf_bind(
        server__, closed, [this](SOCKET sock) { on_disconnect__(sock); }, true);

    sf_bind(
        server__, data_coming,
        [this](SOCKET sock, const pkg_header_t &header,
               const byte_array &data) { on_msg_coming__(sock, header, data); },
        true);
}

inline void
tcp_nat_traversal_server::on_client_require_connect_to_peer_client__(
    tcp_nat_traversal_context_t__ &context) const {
    // 第1步：server获取发起端信息，提交至接受端，context有效字段：connect_id、dest_id、src_id、src_addr
    context.step = 1;
    // 判断来源是否存在
    if (clients__.count(static_cast<const int &>(context.src_id)) == 0) {
        sf_debug("src client not found", context.src_id);
        return;
    }
    // 判断目的是否存在，如果不存在，通知来源
    if (clients__.count(static_cast<const int &>(context.dest_id)) == 0) {
        context.error_code = err_not_exist;
        sf_debug("dst client not found");
        sf_debug("send", skyfire::to_json(context).to_string());
        server__->send(static_cast<int>(context.src_id),
                       err_nat_traversal_err,
                       to_byte_array(skyfire::to_json(context).to_string()));
        return;
    }
    // 获取来源的外网IP和端口，填充到连接上下文
    if (!peer_addr(static_cast<int>(context.src_id), context.src_addr)) {
        context.error_code = err_disconnect;
        sf_debug("send", skyfire::to_json(context).to_string());
        server__->send(static_cast<int>(context.src_id),
                       err_nat_traversal_err,
                       to_byte_array(skyfire::to_json(context).to_string()));
        return;
    }
    sf_debug("notify target addr");
    // 将来源的外网ip端口通知给目的
    sf_debug("send", skyfire::to_json(context).to_string());
    if (!server__->send(static_cast<int>(context.dest_id),
                        type_nat_traversal_new_connect_required,
                        to_byte_array(skyfire::to_json(context).to_string()))) {
        context.error_code = err_remote_err;
        sf_debug("send", skyfire::to_json(context).to_string());
        server__->send(static_cast<int>(context.src_id),
                       err_nat_traversal_err,
                       to_byte_array(skyfire::to_json(context).to_string()));
        return;
    }
}

inline void tcp_nat_traversal_server::on_msg_coming__(
    SOCKET sock, const pkg_header_t &header, const byte_array &data) {
    sf_debug(header.type);
    switch (header.type) {
        case type_nat_traversal_get_list:
            on_update_client_list__(sock);
            break;
        case type_nat_traversal_reg:
            on_client_reg__(sock);
            break;
        case type_nat_traversal_require_connect_peer: {
            sf_debug("recv connection request", to_string(data));
            tcp_nat_traversal_context_t__ context;
            from_json(json::from_string(to_string(data)), context);
            on_client_require_connect_to_peer_client__(context);
        } break;
        case type_nat_traversal_b_reply_addr: {
            sf_debug("recv reply from B");
            tcp_nat_traversal_context_t__ context;
            from_json(json::from_string(to_string(data)), context);
            on_nat_traversal_b_reply_addr(context, sock);
        } break;
        default:
            break;
    }
}

inline void tcp_nat_traversal_server::on_nat_traversal_b_reply_addr(
    tcp_nat_traversal_context_t__ &context, SOCKET sock) const {
    // 第4步，服务器将目的的监听地址信息发送至来源，context有效字段：connect_id、dest_id、src_id、src_addr、dest_addr
    context.step = 4;
    // 判断目的是否存在，不存在，通知来源
    if (clients__.count(static_cast<const int &>(context.dest_id)) == 0) {
        context.error_code = err_not_exist;
        sf_debug("send", skyfire::to_json(context).to_string());
        server__->send(static_cast<int>(context.src_id),
                       err_nat_traversal_err,
                       to_byte_array(skyfire::to_json(context).to_string()));
        return;
    }
    // 判断来源是否存在，如果不存在，通知回复者
    if (clients__.count(static_cast<const int &>(context.src_id)) == 0) {
        context.error_code = err_not_exist;
        sf_debug("send", skyfire::to_json(context).to_string());
        server__->send(static_cast<int>(context.dest_id),
                       err_nat_traversal_err,
                       to_byte_array(skyfire::to_json(context).to_string()));
        return;
    }

    if (!peer_addr(sock, context.dest_addr)) {
        context.error_code = err_disconnect;
        sf_debug("send", skyfire::to_json(context).to_string());
        server__->send(static_cast<int>(context.dest_id),
                       err_nat_traversal_err,
                       to_byte_array(skyfire::to_json(context).to_string()));
        return;
    }
    sf_debug("reply addr of B to A");
    sf_debug("send", skyfire::to_json(context).to_string());
    if (!server__->send(static_cast<int>(context.src_id),
                        type_nat_traversal_server_reply_b_addr,
                        to_byte_array(skyfire::to_json(context).to_string()))) {
        context.error_code = err_remote_err;
        sf_debug("send", skyfire::to_json(context).to_string());
        server__->send(static_cast<int>(context.dest_id),
                       err_nat_traversal_err,
                       to_byte_array(skyfire::to_json(context).to_string()));
        return;
    }
}

inline void tcp_nat_traversal_server::on_update_client_list__(SOCKET sock) {
    const std::unordered_set<unsigned long long> tmp_data{clients__.begin(),
                                                          clients__.end()};
    const auto data = to_byte_array(skyfire::to_json(tmp_data).to_string());
    if (sock == -1) {
        for (auto &p : clients__) {
            sf_debug("send", to_string(data));
            server__->send(p, type_nat_traversal_list, data);
        }
    } else {
        sf_debug("send", to_string(data));
        server__->send(sock, type_nat_traversal_list, data);
    }
}

inline void tcp_nat_traversal_server::on_client_reg__(SOCKET sock) {
    sf_debug("client reg", sock);
    clients__.insert(sock);
    const auto id = static_cast<const unsigned long long int>(sock);
    sf_debug("send", skyfire::to_json(id).to_string());
    server__->send(sock, type_nat_traversal_set_id,
                   to_byte_array(skyfire::to_json(id).to_string()));
    on_update_client_list__();
}

inline void tcp_nat_traversal_server::on_disconnect__(SOCKET sock) {
    clients__.erase(sock);
    on_update_client_list__();
}
}    // namespace skyfire
#pragma clang diagnostic pop