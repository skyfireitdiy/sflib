#pragma once

#include "sf_component_utils.h"
#include "sf_nocopy.h"
#include "sf_object.h"
#include "sf_type.h"
#include "sf_netutils.h"

#include "sf_msg_bus_client.h"
#include "sf_rpcclient.h"

#include <string>
#include <map>


namespace skyfire
{
    class sf_component_client: public sf_nocopy<sf_object>
    {
        SF_REG_SIGNAL(msg_coming, std::string, byte_array);

    private:

        std::shared_ptr<sf_rpcclient> rpc_client__ = sf_rpcclient::make_client();

        std::shared_ptr<sf_msg_bus_client> msg_bus_client__ = sf_msg_bus_client::make_client();

        std::string token__;

    public:
        static std::shared_ptr<sf_component_client> make_client();

        bool connect_to_server(const std::string& ip, unsigned short port);

        bool reg_component(std::string name);

        std::list<std::string> get_component_list(std::string token);

        void clear_private_area();
        byte_array get_private_data(std::string key);
        void set_private_data(std::string key, byte_array value);
        void delete_private_data(std::string key);
        bool has_private_data(std::string key) ;

        bool save_private_data();
        void load_private_data();


        void unreg_component(std::string name);
        // void clear_public_area();
        byte_array get_public_data(std::string key);
        void set_public_data(std::string key, byte_array value);
        void delete_public_data(std::string key);
        bool has_public_data(std::string key) ;
        addr_info_t get_msg_bus_server_addr();

        bool save_data();
        void load_data();

        bool save_public_data();
        void load_public_data();

        void send_msg(const std::string& type, const byte_array & data);
    };
}