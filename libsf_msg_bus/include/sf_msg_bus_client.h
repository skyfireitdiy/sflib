#pragma once

#include "sf_object.h"
#include "sf_type.h"
#include "sf_tcpclient.h"
#include "sf_msg_bus_utils.h"


namespace skyfire
{
    class sf_msg_bus_client : public sf_nocopy<sf_object>
    {
        SF_REG_SIGNAL(msg_come, std::string, byte_array);

    public:

        static std::shared_ptr<sf_msg_bus_client> make_client()
        {
            return std::make_shared<sf_msg_bus_client>();
        }


        sf_msg_bus_client()
        {
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

        ~sf_msg_bus_client()
        {
            close();
        }

        void reg_msg_to_bus(const std::string& type)
        {
            p_client__->send(msg_bus_reg_type_single, sf_serialize(type));
        }

        void reg_msg_to_bus(const std::vector<std::string> &types)
        {
            p_client__->send(msg_bus_reg_type_multi, sf_serialize(types));
        }

        bool connect_to_bus(const std::string &ip, unsigned short port)
        {
            return p_client__->connect(ip,port);
        }

        void send_msg(const std::string& type, const byte_array& data)
        {
            msg_bus_t msg;
            msg.type = type;
            msg.data = data;
            auto send_data = sf_serialize(msg);
            p_client__->send(msg_bus_new_msg, send_data);
        }

        void close()
        {
            p_client__->close();
        }

    private:
        std::shared_ptr<sf_tcpclient> p_client__ = sf_tcpclient::make_client();

        void on_reg_data__(const pkg_header_t& header, const byte_array& data)
        {
            if(header.type == msg_bus_new_msg)
            {
                msg_bus_t msg_data;
                sf_deserialize(data, msg_data, 0);
                msg_come(msg_data.type, msg_data.data);
            }
        }

    };
}

