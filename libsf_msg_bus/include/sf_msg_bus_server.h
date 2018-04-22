#pragma once

#include <string>

#include "sf_msg_bus_utils.h"
#include "sf_type.h"
#include "sf_object.h"
#include "sf_tcpserver.h"
#include "sf_serialize.h"

namespace skyfire
{

    class sf_msg_bus_server : public sf_nocopy<sf_object>
    {
        SF_REG_SIGNAL(msg_come, SOCKET, std::string, byte_array);

    public:

        static std::shared_ptr<sf_msg_bus_server> make_server()
        {
            return std::make_shared<sf_msg_bus_server>();
        }

        sf_msg_bus_server()
        {
            sf_bind_signal(
                    p_server__,
                    data_coming,
                    std::bind(
                            &sf_msg_bus_server::on_reg_data__,
                            this,
                            std::placeholders::_1,
                            std::placeholders::_2,
                            std::placeholders::_3
                    ),
                    true
            );
            sf_bind_signal(
                    p_server__,
                    closed,
                    std::bind(
                            &sf_msg_bus_server::on_disconnect__,
                            this,
                            std::placeholders::_1
                    ),
                    true
            );
        }

        ~sf_msg_bus_server()
        {
            close();
        }

        bool listen(const std::string &ip, unsigned short port)
        {
            if(!p_server__->listen(ip, port))
            {
                return false;
            }

        }

        void close()
        {
            p_server__->close();
            msg_map__.clear();
        }

        void clear_client()
        {
            msg_map__.clear();
        }

        void send_msg(const std::string& type, const byte_array& data)
        {
            msg_bus_t msg;
            msg.type = type;
            msg.data = data;
            auto send_data = sf_serialize(msg);
            if(msg_map__.count(type) != 0)
            {
                for(auto &sock : msg_map__[type])
                {
                    p_server__->send(sock, msg_bus_new_msg, send_data);
                }
            }
        }

    private:
        std::shared_ptr<sf_tcpserver> p_server__ = sf_tcpserver::make_server();

        std::map<std::string, std::list<SOCKET>> msg_map__;

        void reg_msg__(SOCKET sock, const std::string &msg_name)
        {
            if(msg_map__.count(msg_name) == 0)
            {
                msg_map__[msg_name] = std::list<SOCKET>();
                msg_map__[msg_name].push_back(sock);
                return;
            }
            if(std::find(
                    msg_map__[msg_name].begin(),
                    msg_map__[msg_name].end(),
                    sock
            ) != msg_map__[msg_name].end()
                    )
            {
                msg_map__[msg_name].push_back(sock);
            }
        }

        void on_reg_data__(SOCKET sock, const pkg_header_t& header, const byte_array& data)
        {
            if(header.type == msg_bus_reg_type_single)
            {
                std::string name;
                sf_deserialize(data, name, 0);
                reg_msg__(sock, name);
            }
            else if(header.type == msg_bus_reg_type_multi)
            {
                std::vector<std::string> names;
                sf_deserialize(data, names, 0);
                for(auto &p:names)
                {
                    reg_msg__(sock, p);
                }
            }
            else if(header.type == msg_bus_new_msg)
            {
                msg_bus_t msg_data;
                sf_deserialize(data, msg_data, 0);
                msg_come(sock, msg_data.type, msg_data.data);
                send_msg(msg_data.type, msg_data.data);
            }
        }

        void on_disconnect__(SOCKET sock)
        {
            for(auto &p : msg_map__)
            {
                std::remove(p.second.begin(),p.second.end(),sock);
            }
        }
    };
}