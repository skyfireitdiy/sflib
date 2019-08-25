
/**
* @version 1.0.0
* @author skyfire
* @mail skyfireitdiy@hotmail.com
* @see http://github.com/skyfireitdiy/sflib
* @file sf_msg_bus_server.hpp

* sflib第一版本发布
* 版本号1.0.0
* 发布日期：2018-10-22
*/

/*
 * sf_msg_bus_server 消息总线服务器
 */
#pragma once
#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCUnusedGlobalDeclarationInspection"

#include "sf_msg_bus_server.h"
#include "sf_msg_bus_utils.h"
#include "sf_tcp_server.h"
#include "sf_tcp_server_interface.hpp"

namespace skyfire {

inline void sf_msg_bus_server::on_disconnect__(const SOCKET sock) {
    std::vector<std::string> remove_msg;
    for (auto &p : msg_map__) {
        // ReSharper disable once CppNoDiscardExpression
        [[maybe_unused]] auto tmp =
            std::remove(p.second.begin(), p.second.end(), sock);
        if (p.second.empty()) remove_msg.push_back(p.first);
    }
    for (auto &p : remove_msg) {
        msg_map__.erase(p);
    }
}

inline void sf_msg_bus_server::unreg_msg__(SOCKET sock,
                                           const std::string &msg) {
    if (msg_map__.count(msg) != 0) {
        msg_map__[msg].remove(sock);
    }
}

inline void sf_msg_bus_server::on_reg_data__(SOCKET sock,
                                             const sf_pkg_header_t &header,
                                             const byte_array &data) {
    if (header.type == msg_bus_reg_type_single) {
        std::string name;
        from_json(sf_json::from_string(to_string(data)), name);
        reg_msg__(sock, name);
    } else if (header.type == msg_bus_reg_type_multi) {
        std::vector<std::string> names;
        from_json(sf_json::from_string(to_string(data)), names);
        for (auto &p : names) {
            reg_msg__(sock, p);
        }
    } else if (header.type == msg_bus_new_msg) {
        sf_msg_bus_t msg_data;
        from_json(sf_json::from_string(to_string(data)), msg_data);
        msg_come(sock, msg_data.type, msg_data.data);
        // TODO 服务器是否需要转发所有的消息？
        send_msg(msg_data.type, msg_data.data);
    } else if (header.type == msg_bus_unreg_single) {
        std::string name;
        from_json(sf_json::from_string(to_string(data)), name);
        unreg_msg__(sock, name);
    } else if (header.type == msg_bus_unreg_multi) {
        std::vector<std::string> names;
        from_json(sf_json::from_string(to_string(data)), names);
        for (auto &p : names) {
            unreg_msg__(sock, p);
        }
    }
}

inline void sf_msg_bus_server::reg_msg__(SOCKET sock,
                                         const std::string &msg_name) {
    if (msg_map__.count(msg_name) == 0) {
        msg_map__[msg_name] = std::list<SOCKET>();
        msg_map__[msg_name].push_back(sock);
        return;
    }
    if (std::find(msg_map__[msg_name].begin(), msg_map__[msg_name].end(),
                  sock) != msg_map__[msg_name].end()) {
        msg_map__[msg_name].push_back(sock);
    }
}

inline void sf_msg_bus_server::send_msg(const std::string &type,
                                        const byte_array &data) {
    sf_msg_bus_t msg;
    msg.type = type;
    msg.data = data;
    const auto send_data = to_byte_array(skyfire::to_json(msg).to_string());
    if (msg_map__.count(type) != 0) {
        for (auto &sock : msg_map__[type]) {
            p_server__->send(sock, msg_bus_new_msg, send_data);
        }
    }
}

inline void sf_msg_bus_server::clear_client() { msg_map__.clear(); }

inline void sf_msg_bus_server::close() {
    p_server__->close();
    msg_map__.clear();
}

inline bool sf_msg_bus_server::listen(const std::string &ip,
                                      unsigned short port) const {
    return p_server__->listen(ip, port);
}

inline sf_msg_bus_server::~sf_msg_bus_server() { close(); }

inline sf_msg_bus_server::sf_msg_bus_server() {
    sf_bind_signal(
        p_server__, data_coming,
        [this](SOCKET sock, const sf_pkg_header_t &header,
               const byte_array &data) { on_reg_data__(sock, header, data); },
        true);
    sf_bind_signal(
        p_server__, closed, [this](SOCKET sock) { on_disconnect__(sock); },
        true);
}

inline bool sf_msg_bus_server::server_addr(sf_addr_info_t &addr) const {
    return p_server__->server_addr(addr);
}
}    // namespace skyfire

#pragma clang diagnostic pop