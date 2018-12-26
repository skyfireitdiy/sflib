
/**
* @version 1.0.0
* @author skyfire
* @mail skyfireitdiy@hotmail.com
* @see http://github.com/skyfireitdiy/sflib
* @file sf_tcp_nat_traversal_server.hpp

* sflib第一版本发布
* 版本号1.0.0
* 发布日期：2018-10-22
*/
#pragma once
#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCDFAInspection"


#include "sf_tcp_nat_traversal_server.h"

namespace skyfire {


    inline void sf_tcp_nat_traversal_server::on_new_connection__(SOCKET sock)
    {
        // TODO 做一些处理
        sf_debug("new connection", sock);
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
            on_new_connection__(sock);
        }, true);
        sf_bind_signal(server__, closed, [=](SOCKET sock) {
            on_disconnect__(sock);
        }, true);

        sf_bind_signal(server__, data_coming, [=](SOCKET sock, const sf_pkg_header_t &header, const byte_array &data) {
            on_msg_coming__(sock, header, data);
        }, true);
    }

    inline std::shared_ptr <sf_tcp_nat_traversal_server> sf_tcp_nat_traversal_server::make_server() {
        return std::make_shared<sf_tcp_nat_traversal_server>();
    }

    inline void
    sf_tcp_nat_traversal_server::on_client_require_connect_to_peer_client__(sf_tcp_nat_traversal_context_t__ &context) const
    {
        // 第1步：server获取发起端信息，提交至接受端，context有效字段：connect_id、dest_id、src_id、src_addr
        context.step = 1;
        // 判断来源是否存在
        if (clients__.count(static_cast<const int &>(context.src_id)) == 0) {
            return;
        }
        // 判断目的是否存在，如果不存在，通知来源
        if (clients__.count(static_cast<const int &>(context.dest_id)) == 0) {
            context.error_code = sf_err_not_exist;
            server__->send(static_cast<int>(context.src_id), type_nat_traversal_error, to_byte_array(skyfire:: to_json(context)));
            return;
        }
        // 获取来源的外网IP和端口，填充到连接上下文
        if (!get_peer_addr(static_cast<int>(context.src_id), context.src_addr)) {
            context.error_code = sf_err_disconnect;
            server__->send(static_cast<int>(context.src_id), type_nat_traversal_error, to_byte_array(skyfire::to_json(context)));
            return;
        }
        sf_debug("notify target addr");
        // 将来源的外网ip端口通知给目的
        if (!server__->send(static_cast<int>(context.dest_id), type_nat_traversal_new_connect_required, to_byte_array(skyfire::to_json(context)))) {
            context.error_code = sf_err_remote_err;
            server__->send(static_cast<int>(context.src_id), type_nat_traversal_error, to_byte_array(skyfire::to_json(context)));
            return;
        }
    }

    inline void sf_tcp_nat_traversal_server::on_msg_coming__(SOCKET sock, const sf_pkg_header_t &header, const byte_array &data) {
        sf_debug(header.type);
        switch (header.type) {
            case type_nat_traversal_get_list:
                on_update_client_list__(sock);
                break;
            case type_nat_traversal_reg:
                on_client_reg__(sock);
                break;
            case type_nat_traversal_require_connect_peer: {
                sf_debug("recv connection request");
                sf_tcp_nat_traversal_context_t__ context;
				from_json(sf_json::from_string(to_string(data)), context);
                on_client_require_connect_to_peer_client__(context);
            }
                break;
            case type_nat_traversal_b_reply_addr: {
                sf_debug("recv reply from B");
                sf_tcp_nat_traversal_context_t__ context;
				from_json(sf_json::from_string(to_string(data)), context);
                on_nat_traversal_b_reply_addr(context, sock);
            }
                break;
            default:
                break;
        }
    }

    inline void
    sf_tcp_nat_traversal_server::on_nat_traversal_b_reply_addr(sf_tcp_nat_traversal_context_t__ &context, SOCKET sock) const
    {
        // 第4步，服务器将目的的监听地址信息发送至来源，context有效字段：connect_id、dest_id、src_id、src_addr、dest_addr
        context.step = 4;
        // 判断目的是否存在，不存在，通知来源
        if (clients__.count(static_cast<const int &>(context.dest_id)) == 0) {
            context.error_code = sf_err_not_exist;
            server__->send(static_cast<int>(context.src_id), type_nat_traversal_error, to_byte_array(skyfire:: to_json(context)));
            return;
        }
        // 判断来源是否存在，如果不存在，通知回复者
        if (clients__.count(static_cast<const int &>(context.src_id)) == 0) {
            context.error_code = sf_err_not_exist;
            server__->send(static_cast<int>(context.dest_id), type_nat_traversal_error, to_byte_array(skyfire::to_json(context)));
            return;
        }

        if (!get_peer_addr(sock, context.dest_addr)) {
            context.error_code = sf_err_disconnect;
            server__->send(static_cast<int>(context.dest_id), type_nat_traversal_error, to_byte_array(skyfire::to_json(context)));
            return;
        }
        sf_debug("reply addr of B to A");
        if (!server__->send(static_cast<int>(context.src_id), type_nat_traversal_server_reply_b_addr, to_byte_array(skyfire::to_json(context)))) {
            context.error_code = sf_err_remote_err;
            server__->send(static_cast<int>(context.dest_id), type_nat_traversal_error, to_byte_array(skyfire::to_json(context)));
            return;
        }
    }

    inline void sf_tcp_nat_traversal_server::on_update_client_list__(SOCKET sock) {
	    const std::unordered_set<unsigned long long> tmp_data{clients__.begin(), clients__.end()};
	    const auto data = to_byte_array(skyfire::to_json(tmp_data));
        if (sock == -1) {
            for (auto &p:clients__) {
                server__->send(p, type_nat_traversal_list, data);
            }
        } else {
            server__->send(sock, type_nat_traversal_list, data);
        }
    }

    inline void sf_tcp_nat_traversal_server::on_client_reg__(SOCKET sock) {
        clients__.insert(sock);
        const auto id = static_cast<const unsigned long long int>(sock);
        server__->send(sock, type_nat_traversal_set_id, to_byte_array(skyfire::to_json(id)));
        on_update_client_list__();
    }

    inline void sf_tcp_nat_traversal_server::on_disconnect__(SOCKET sock) {
        clients__.erase(sock);
        on_update_client_list__();
    }
}
#pragma clang diagnostic pop