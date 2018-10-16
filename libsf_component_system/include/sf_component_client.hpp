#pragma once


#include "sf_component_utils.h"
#include "sf_nocopy.hpp"
#include "sf_object.hpp"
#include "sf_type.hpp"
#include "sf_netutils.hpp"

#include "sf_msg_bus_client.hpp"
#include "sf_rpcclient.h.hpp"

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
}