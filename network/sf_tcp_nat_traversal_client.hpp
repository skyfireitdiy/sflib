
/**
* @version 1.0.0
* @author skyfire
* @mail skyfireitdiy@hotmail.com
* @see http://github.com/skyfireitdiy/sflib
* @file sf_tcp_nat_traversal_client.hpp

* sflib第一版本发布
* 版本号1.0.0
* 发布日期：2018-10-22
*/
#pragma once
#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCDFAInspection"

#include "sf_tcp_nat_traversal_client.h"

namespace skyfire {

#pragma clang diagnostic push
#pragma ide diagnostic ignored "bugprone-lambda-function-name"
inline void sf_tcp_nat_traversal_client::on_new_connect_required__(
    sf_tcp_nat_traversal_context_t__ &context) {
    // 创建连接上下文
    connect_context_map__[context.connect_id] = sf_p2p_connect_context_t__{};
    // 保存连接上下文
    connect_context_map__[context.connect_id].tcp_nat_traversal_context =
        context;

    // 第2步，接受端尝试连接A的外网端口，如果连上，一切ok，context有效字段：connect_id、dest_id、src_id、src_addr
    connect_context_map__[context.connect_id].tcp_nat_traversal_context.step =
        2;

    // 建立客户端1
    connect_context_map__[context.connect_id].point_b_client_1 =
        sf_tcp_client::make_instance();

    sf_addr_info_t server_addr;
    if (!local_addr(client__->raw_socket(), server_addr)) {
        context.error_code = sf_err_disconnect;
        client__->send(
            type_nat_traversal_error,
            to_byte_array(
                skyfire::to_json(connect_context_map__[context.connect_id]
                                     .tcp_nat_traversal_context)
                    .to_string()));
        return;
    }

    // 生成一个自动端口
    const auto auto_port = static_cast<unsigned short>(
        sf_random::instance()->rand_int(1025, 65535));

    sf_debug("local addr", server_addr.ip, auto_port);

    // 绑定
    if (!connect_context_map__[context.connect_id].point_b_client_1->bind(
            server_addr.ip, auto_port)) {
        context.error_code = sf_err_bind_err;
        client__->send(
            type_nat_traversal_error,
            to_byte_array(
                skyfire::to_json(connect_context_map__[context.connect_id]
                                     .tcp_nat_traversal_context)
                    .to_string()));
        return;
    }

    sf_debug("try connect to A",
             connect_context_map__[context.connect_id]
                 .tcp_nat_traversal_context.src_addr.ip,
             connect_context_map__[context.connect_id]
                 .tcp_nat_traversal_context.src_addr.port);
    // 尝试连接a的外网ip端口，留下路由信息（一般会失败，但有时会直接成功）
    if (connect_context_map__[context.connect_id]
            .point_b_client_1->connect_to_server(
                connect_context_map__[context.connect_id]
                    .tcp_nat_traversal_context.src_addr.ip,
                connect_context_map__[context.connect_id]
                    .tcp_nat_traversal_context.src_addr.port)) {
        // TODO 为了维持操作统一，成功暂时也不做处理
        sf_debug("connect to source addr");
    }
    // 连接失败，B建立服务器2

    // 第3步：接受端建立socket，发送信息，便于server获取到接受端地址信息，同时监听端口，context有效字段：connect_id、dest_id、src_id、src_addr
    connect_context_map__[context.connect_id].tcp_nat_traversal_context.step =
        3;

    connect_context_map__[context.connect_id].point_b_client_2 =
        sf_tcp_client::make_instance();

    // 将客户端2绑定至于客户端1相同的ip端口
    if (!connect_context_map__[context.connect_id].point_b_client_2->bind(
            server_addr.ip, auto_port)) {
        context.error_code = sf_err_bind_err;
        client__->send(
            type_nat_traversal_error,
            to_byte_array(
                skyfire::to_json(connect_context_map__[context.connect_id]
                                     .tcp_nat_traversal_context)
                    .to_string()));
        return;
    }

    // 监听端口，等待连接
    connect_context_map__[context.connect_id].point_b_server =
        sf_tcp_server::make_instance(context.raw);
    if (!connect_context_map__[context.connect_id].point_b_server->listen(
            server_addr.ip, auto_port)) {
        context.error_code = sf_err_disconnect;
        client__->send(
            type_nat_traversal_error,
            to_byte_array(
                skyfire::to_json(connect_context_map__[context.connect_id]
                                     .tcp_nat_traversal_context)
                    .to_string()));
        return;
    }

    // lambda 里面需要此绑定id，所以需要传递指针
    std::shared_ptr<int> tmp_bind_id = std::make_shared<int>();
    *tmp_bind_id = sf_bind_signal(
        connect_context_map__[context.connect_id].point_b_server,
        new_connection, ([this, context, tmp_bind_id](SOCKET sock) {
            sf_debug("B new connection,NAT conection built");
            std::shared_ptr<sf_tcp_nat_traversal_connection> connection(
                new sf_tcp_nat_traversal_connection(
                    nullptr,
                    connect_context_map__[context.connect_id].point_b_server,
                    sock, context.connect_id,
                    sf_tcp_nat_traversal_connection_type::type_server_valid));
            new_nat_connection(connection, context.connect_id);
            // 解除绑定
            sf_unbind_signal(
                connect_context_map__[context.connect_id].point_b_server,
                new_connection, *tmp_bind_id);
            sf_debug("unbind id",
                     *tmp_bind_id);    // NOLINT(bugprone-lambda-function-name)
            connect_context_map__.erase(context.connect_id);
        }),
        true);

    sf_debug("bind id", *tmp_bind_id);

    // 使用客户端2连接至server，便于server获取目的的外网ip端口
    if (!connect_context_map__[context.connect_id]
             .point_b_client_2->connect_to_server(server_addr__.ip,
                                                  server_addr__.port)) {
        context.error_code = sf_err_disconnect;
        client__->send(
            type_nat_traversal_error,
            to_byte_array(
                skyfire::to_json(connect_context_map__[context.connect_id]
                                     .tcp_nat_traversal_context)
                    .to_string()));
        return;
    }

    // 向server发送信息，server获取ip端口（此时已在路由器留下记录）
    sf_debug("reply server");
    sf_debug("send", "type_nat_traversal_b_reply_addr");
    if (!connect_context_map__[context.connect_id].point_b_client_2->send(
            type_nat_traversal_b_reply_addr,
            to_byte_array(
                skyfire::to_json(connect_context_map__[context.connect_id]
                                     .tcp_nat_traversal_context)
                    .to_string()))) {
        context.error_code = sf_err_disconnect;
        client__->send(
            type_nat_traversal_error,
            to_byte_array(
                skyfire::to_json(connect_context_map__[context.connect_id]
                                     .tcp_nat_traversal_context)
                    .to_string()));
        return;
    }
}
#pragma clang diagnostic pop

inline void sf_tcp_nat_traversal_client::close() const { client__->close(); }

inline std::string sf_tcp_nat_traversal_client::connect_to_peer(
    unsigned long long peer_id, bool raw) {
    // 生成一个连接上下文
    sf_p2p_connect_context_t__ tmp_p2p_conn_context;
    // 第0步：发起端主动连接server，context有效字段：connect_id、dest_id、src_id
    tmp_p2p_conn_context.tcp_nat_traversal_context.step = 0;
    // 生成随机连接id
    tmp_p2p_conn_context.tcp_nat_traversal_context.connect_id =
        sf_random::instance()->uuid_str();
    // 指定对方id
    tmp_p2p_conn_context.tcp_nat_traversal_context.dest_id = peer_id;
    // 指定自身id
    tmp_p2p_conn_context.tcp_nat_traversal_context.src_id = self_id__;
    // 原始数据传输
    tmp_p2p_conn_context.tcp_nat_traversal_context.raw = raw;
    // 生成连接a客户端
    tmp_p2p_conn_context.point_a_client_1 = sf_tcp_client::make_instance();
    // 尝试连接服务器
    if (tmp_p2p_conn_context.point_a_client_1->connect_to_server(
            server_addr__.ip, server_addr__.port)) {
        sf_addr_info_t addr;
        // 获取到本机要连接外网的ip端口，保存至连接上下文
        if (!local_addr(tmp_p2p_conn_context.point_a_client_1->raw_socket(),
                        addr)) {
            sf_error("get local ip port error");
            return "";
        }

        tmp_p2p_conn_context.point_a_server = sf_tcp_server::make_instance();
        // 复用刚才断开的连接1端口号
        if (!tmp_p2p_conn_context.point_a_server->listen(addr.ip, addr.port)) {
            sf_error("listen local ip port error", addr.ip, addr.port);
            return "";
        }

        sf_debug("start connect");
        // 发起连接请求
        sf_debug("send", "type_nat_traversal_require_connect_peer");
        if (tmp_p2p_conn_context.point_a_client_1->send(
                type_nat_traversal_require_connect_peer,
                to_byte_array(
                    skyfire::to_json(
                        tmp_p2p_conn_context.tcp_nat_traversal_context)
                        .to_string()))) {
            // 保存连接上下文
            connect_context_map__[tmp_p2p_conn_context.tcp_nat_traversal_context
                                      .connect_id] = tmp_p2p_conn_context;
            return tmp_p2p_conn_context.tcp_nat_traversal_context.connect_id;
        }
    }
    return "";
}

inline std::unordered_set<unsigned long long int>
sf_tcp_nat_traversal_client::clients() const {
    return client_list__;
}

inline bool sf_tcp_nat_traversal_client::connect_to_server(
    const std::string &ip, unsigned short port) {
    server_addr__.ip = ip;
    server_addr__.port = port;
    if (client__->connect_to_server(ip, port)) {
        sf_debug("send", "type_nat_traversal_reg");
        client__->send(type_nat_traversal_reg, byte_array());
        return true;
    }
    return false;
}

inline sf_tcp_nat_traversal_client::sf_tcp_nat_traversal_client() {
    sf_bind_signal(
        client__, data_coming,
        [this](const sf_pkg_header_t &header, const byte_array &data) {
            on_client_data_coming__(header, data);
        },
        true);
    sf_bind_signal(
        client__, closed, [this] { close(); }, true);
}

inline unsigned long long int sf_tcp_nat_traversal_client::id() const {
    return self_id__;
}

inline void sf_tcp_nat_traversal_client::on_client_data_coming__(
    const sf_pkg_header_t &header, const byte_array &data) {
    sf_debug(header.type);
    switch (header.type) {
        case type_nat_traversal_list:
            sf_debug("recv", "type_nat_traversal_list", to_string(data));
            from_json(sf_json::from_string(to_string(data)), client_list__);
            break;
        case type_nat_traversal_set_id:
            sf_debug("recv", "type_nat_traversal_set_id", to_string(data));
            from_json(sf_json::from_string(to_string(data)), self_id__);
            break;
        case type_nat_traversal_new_connect_required: {
            sf_debug("recv", "type_nat_traversal_new_connect_required",
                     to_string(data));
            sf_tcp_nat_traversal_context_t__ context;
            from_json(sf_json::from_string(to_string(data)), context);
            on_new_connect_required__(context);
        } break;
        case type_nat_traversal_server_reply_b_addr: {
            sf_debug("recv", "type_nat_traversal_server_reply_b_addr",
                     to_string(data));
            sf_tcp_nat_traversal_context_t__ context;
            from_json(sf_json::from_string(to_string(data)), context);
            on_server_reply_b_addr(context);
        } break;
        default:
            break;
    }
}

inline void sf_tcp_nat_traversal_client::on_client_close__() {
    client_list__.clear();
}

inline void sf_tcp_nat_traversal_client::on_server_reply_b_addr(
    sf_tcp_nat_traversal_context_t__ &context) {
    // 第5步，a连接b的监听端口,context有效字段：connect_id、dest_id、src_id、src_addr、dest_addr
    context.step = 5;
    if (connect_context_map__.count(context.connect_id) == 0) {
        context.error_code = sf_err_not_exist;
        sf_debug("send", "type_nat_traversal_error",
                 skyfire::to_json(context).to_string());
        client__->send(type_nat_traversal_error,
                       to_byte_array(skyfire::to_json(context).to_string()));
        return;
    }
    connect_context_map__[context.connect_id].tcp_nat_traversal_context =
        context;
    connect_context_map__[context.connect_id].point_a_client_2 =
        sf_tcp_client::make_instance(context.raw);
    sf_debug("connect to B");
    if (connect_context_map__[context.connect_id]
            .point_a_client_2->connect_to_server(context.dest_addr.ip,
                                                 context.dest_addr.port)) {
        std::shared_ptr<sf_tcp_nat_traversal_connection> connection(
            new sf_tcp_nat_traversal_connection(
                connect_context_map__[context.connect_id].point_a_client_2,
                nullptr, -1, context.connect_id,
                sf_tcp_nat_traversal_connection_type::type_client_valid));
        sf_debug("NAT success");
        new_nat_connection(connection, context.connect_id);
        connect_context_map__.erase(context.connect_id);
    } else {
        context.error_code = sf_err_disconnect;
        sf_debug("send", "type_nat_traversal_error",
                 skyfire::to_json(context).to_string());
        client__->send(type_nat_traversal_error,
                       to_byte_array(skyfire::to_json(context).to_string()));
        return;
    }
}
}    // namespace skyfire
#pragma clang diagnostic pop