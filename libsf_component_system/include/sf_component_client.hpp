#pragma once

#include "sf_component_client.h"
#include "sf_component_utils.h"
#include "sf_nocopy.hpp"
#include "sf_object.hpp"
#include "sf_type.hpp"
#include "sf_netutils.hpp"

#include "sf_msg_bus_client.hpp"
#include "sf_rpcclient.hpp"

namespace skyfire
{
    inline std::shared_ptr<sf_component_client> sf_component_client::make_client()
    {
        return std::shared_ptr<sf_component_client>(new sf_component_client);
    }

    inline bool sf_component_client::connect_to_server(const std::string &ip, unsigned short port)
    {
        if(!rpc_client__->connect_to_server(ip,port))
        {
            return false;
        }
        return true;
    }

    inline bool sf_component_client::reg_component(std::string name)
    {
        auto token = rpc_client__->call<std::string>("reg_component",name);
        if(!token)
        {
            return false;
        }
        if((*token).empty())
        {
            return false;
        }
        token__ = *token;
        auto addr = rpc_client__->call<addr_info_t>("get_msg_bus_server_addr",token__);
        if(!addr)
        {
            return false;
        }
        if((*addr).port == 0)
        {
            return false;
        }
        return msg_bus_client__->connect_to_server((*addr).ip,(*addr).port);
    }

    inline byte_array sf_component_client::get_public_data(std::string key)
    {
        auto ret = rpc_client__->call<byte_array>(token__, key);
        if(!ret)
        {
            return byte_array();
        }
        return *ret;
    }
}