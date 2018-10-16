/*
 * sf_msg_bus_client 消息总线客户端
 */

#pragma once


#include "sf_msg_bus_client.h"

namespace skyfire
{
    inline std::shared_ptr<sf_msg_bus_client> sf_msg_bus_client::make_client() {
        return std::make_shared<sf_msg_bus_client>();
    }

    inline sf_msg_bus_client::sf_msg_bus_client() {
        sf_bind_signal(
                p_client__,
                data_coming,
                std::bind(
                        &sf_msg_bus_client::on_reg_data__,
                        this,
                        std::placeholders::_1,
                        std::placeholders::_2
                ),
                true
        );
    }

    inline sf_msg_bus_client::~sf_msg_bus_client() {
        close();
    }

    inline void sf_msg_bus_client::reg_msg_to_bus(const std::string &type) {
        p_client__->send(msg_bus_reg_type_single, sf_serialize_binary(type));
    }

    inline void sf_msg_bus_client::reg_msg_to_bus(const std::vector<std::string> &types) {
        p_client__->send(msg_bus_reg_type_multi, sf_serialize_binary(types));
    }

    inline void sf_msg_bus_client::unreg_msg_to_bus(const std::string &type) {
        p_client__->send(msg_bus_unreg_single, sf_serialize_binary(type));
    }

    inline void sf_msg_bus_client::unreg_msg_to_bus(const std::vector<std::string> &types) {
        p_client__->send(msg_bus_unreg_multi, sf_serialize_binary(types));
    }

    inline bool sf_msg_bus_client::connect_to_server(const std::string &ip, unsigned short port) {
        return p_client__->connect_to_server(ip,port);
    }

    inline void sf_msg_bus_client::send_msg(const std::string &type, const byte_array &data) {
        msg_bus_t msg;
        msg.type = type;
        msg.data = data;
        auto send_data = sf_serialize_binary(msg);
        p_client__->send(msg_bus_new_msg, send_data);
    }

    inline void sf_msg_bus_client::close() {
        p_client__->close();
    }

    inline void sf_msg_bus_client::on_reg_data__(const pkg_header_t &header, const byte_array &data) {
        if(header.type == msg_bus_new_msg)
        {
            msg_bus_t msg_data;
            sf_deserialize_binary(data, msg_data, 0);
            msg_come(msg_data.type, msg_data.data);
        }
    }

    inline bool sf_msg_bus_client::get_local_addr(addr_info_t &addr)
    {
        return p_client__->get_local_addr(addr);
    }

    bool sf_msg_bus_client::get_peer_addr(addr_info_t &addr)
    {
        return p_client__->get_peer_addr(addr);
    }
}

