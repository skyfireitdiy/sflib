/*
 * sf_msg_bus_server 消息总线服务器
 */

#pragma once

#include "sf_msg_bus_server.h"

namespace skyfire {

    inline std::shared_ptr<sf_msg_bus_server> sf_msg_bus_server::make_server() {
        return std::make_shared<sf_msg_bus_server>();
    }

    inline void sf_msg_bus_server::on_disconnect__(SOCKET sock) {
        std::vector<std::string> remove_msg;
        for (auto &p : msg_map__) {
            std::remove(p.second.begin(), p.second.end(), sock);
            if (p.second.empty())
                remove_msg.push_back(p.first);
        }
        for (auto &p: remove_msg) {
            msg_map__.erase(p);
        }
    }

    inline void sf_msg_bus_server::unreg_msg__(SOCKET sock, const std::string &msg) {
        if (msg_map__.count(msg) != 0) {
            msg_map__[msg].remove(sock);
        }
    }

    inline void sf_msg_bus_server::on_reg_data__(SOCKET sock, const pkg_header_t &header, const byte_array &data) {
        if (header.type == msg_bus_reg_type_single) {
            std::string name;
            sf_deserialize_binary(data, name, 0);
            reg_msg__(sock, name);
        } else if (header.type == msg_bus_reg_type_multi) {
            std::vector<std::string> names;
            sf_deserialize_binary(data, names, 0);
            for (auto &p:names) {
                reg_msg__(sock, p);
            }
        } else if (header.type == msg_bus_new_msg) {
            msg_bus_t msg_data;
            sf_deserialize_binary(data, msg_data, 0);
            msg_come(sock, msg_data.type, msg_data.data);
            // TODO 服务器是否需要转发所有的消息？
            send_msg(msg_data.type, msg_data.data);
        } else if (header.type == msg_bus_unreg_single) {
            std::string name;
            sf_deserialize_binary(data, name, 0);
            unreg_msg__(sock, name);
        } else if (header.type == msg_bus_unreg_multi) {
            std::vector<std::string> names;
            sf_deserialize_binary(data, names, 0);
            for (auto &p:names) {
                unreg_msg__(sock, p);
            }
        }
    }

    inline void sf_msg_bus_server::reg_msg__(SOCKET sock, const std::string &msg_name) {
        if (msg_map__.count(msg_name) == 0) {
            msg_map__[msg_name] = std::list<SOCKET>();
            msg_map__[msg_name].push_back(sock);
            return;
        }
        if (std::find(
                msg_map__[msg_name].begin(),
                msg_map__[msg_name].end(),
                sock
        ) != msg_map__[msg_name].end()
                ) {
            msg_map__[msg_name].push_back(sock);
        }
    }

    inline void sf_msg_bus_server::send_msg(const std::string &type, const byte_array &data) {
        msg_bus_t msg;
        msg.type = type;
        msg.data = data;
        auto send_data = sf_serialize_binary(msg);
        if (msg_map__.count(type) != 0) {
            for (auto &sock : msg_map__[type]) {
                p_server__->send(sock, msg_bus_new_msg, send_data);
            }
        }
    }

    inline void sf_msg_bus_server::clear_client() {
        msg_map__.clear();
    }

    inline void sf_msg_bus_server::close() {
        p_server__->close();
        msg_map__.clear();
    }

    inline bool sf_msg_bus_server::listen(const std::string &ip, unsigned short port) {
        return p_server__->listen(ip, port);
    }

    inline sf_msg_bus_server::~sf_msg_bus_server() {
        close();
    }

    inline sf_msg_bus_server::sf_msg_bus_server() {
        sf_bind_signal(
                p_server__,
                data_coming,
                [=](SOCKET sock, const pkg_header_t &header, const byte_array &data) {
                    on_reg_data__(sock, header, data);
                },
                true
        );
        sf_bind_signal(
                p_server__,
                closed,
                [=](SOCKET sock) {
                    on_disconnect__(sock);
                },
                true
        );
    }
}
