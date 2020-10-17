
/**
* @version 1.0.0
* @author skyfire
* @file sf_msg_bus_client.hpp
*/

/*
 * msg_bus_client 消息总线客户端
 */
#pragma once
#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCUnusedGlobalDeclarationInspection"

#include "sf_msg_bus_client.h"
#include "sf_msg_bus_utils.h"

namespace skyfire {
inline msg_bus_client::msg_bus_client() {
    sf_bind(
        p_client__, data_coming,
        std::function<void(const pkg_header_t &, const byte_array &)>(
            std::bind(&msg_bus_client::on_reg_data__, this,
                      std::placeholders::_1, std::placeholders::_2)),
        true);
}

inline msg_bus_client::~msg_bus_client() { close(); }

inline void msg_bus_client::reg_msg_to_bus(const std::string &type) const {
    p_client__->send(msg_bus_reg_type_single,
                     to_byte_array(skyfire::to_json(type).to_string()));
}

inline void msg_bus_client::reg_msg_to_bus(
    const std::vector<std::string> &types) const {
    p_client__->send(msg_bus_reg_type_multi,
                     to_byte_array(skyfire::to_json(types).to_string()));
}

inline void msg_bus_client::unreg_msg_to_bus(const std::string &type) const {
    p_client__->send(msg_bus_unreg_single,
                     to_byte_array(skyfire::to_json(type).to_string()));
}

inline void msg_bus_client::unreg_msg_to_bus(
    const std::vector<std::string> &types) const {
    p_client__->send(msg_bus_unreg_multi,
                     to_byte_array(skyfire::to_json(types).to_string()));
}

inline bool msg_bus_client::connect_to_server(const std::string &ip,
                                                 unsigned short port) const {
    return p_client__->connect_to_server(ip, port);
}

inline void msg_bus_client::send_msg(const std::string &type,
                                        const byte_array &data) const {
    msg_bus_t msg;
    msg.type = type;
    msg.data = data;
    const auto send_data = to_byte_array(skyfire::to_json(msg).to_string());
    p_client__->send(msg_bus_new_msg, send_data);
}

inline void msg_bus_client::close() const { p_client__->close(); }

inline void msg_bus_client::on_reg_data__(const pkg_header_t &header,
                                             const byte_array &data) {
    if (header.type == msg_bus_new_msg) {
        msg_bus_t msg_data;
        from_json(json::from_string(to_string(data)), msg_data);
        msg_come(msg_data.type, msg_data.data);
    }
}

inline bool msg_bus_client::local_addr(addr_info_t &addr) const {
    return p_client__->local_addr(addr);
}

inline bool msg_bus_client::peer_addr(addr_info_t &addr) const {
    return p_client__->peer_addr(addr);
}
}    // namespace skyfire

#pragma clang diagnostic pop