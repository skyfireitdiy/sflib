#include "sf_component_server.h"

#include "sf_nocopy.hpp"
#include "sf_object.hpp"
#include "sf_type.hpp"
#include "sf_rpcserver.hpp"
#include "sf_utils.hpp"

namespace skyfire
{
    inline byte_array sf_component_server::get_private_data(SOCKET sock, std::string key)
    {
        std::shared_lock<std::shared_mutex> lck(mu_private__);
        if(component_context__.count(sock)==0)
        {
            return byte_array();
        }
        if(component_context__[sock].private_data.count(key) == 0)
        {
            return byte_array();
        }
        return component_context__[sock].private_data[key];
    }

    inline void sf_component_server::delete_private_data(SOCKET sock, std::string key)
    {
        std::unique_lock<std::shared_mutex> lck(mu_private__);
        if(component_context__.count(sock)==0)
        {
            return;
        }
        component_context__[sock].private_data.erase(key);
    }

    inline void sf_component_server::clear_private_data(SOCKET sock)
    {
        std::unique_lock<std::shared_mutex> lck(mu_private__);
        if(component_context__.count(sock)==0)
        {
            return;
        }
        component_context__[sock].private_data.clear();
    }

    inline void sf_component_server::set_private_data(SOCKET sock, std::string key, byte_array value)
    {
        std::unique_lock<std::shared_mutex> lck(mu_private__);
        if(component_context__.count(sock)==0)
        {
            // TODO 异常情况
            return;
        }
        component_context__[sock].private_data[key]=value;
    }

    inline bool sf_component_server::has_private_data(SOCKET sock, std::string key)
    {
        std::shared_lock<std::shared_mutex> lck(mu_private__);
        if(component_context__.count(sock)==0)
        {
            return false;
        }
        return component_context__[sock].private_data.count(key)!=0;
    }

    inline bool sf_component_server::reg_component(SOCKET sock, std::string name)
    {
        std::unique_lock<std::shared_mutex> lck(mu_private__);
        if(component_context__.count(sock)!=0)
        {
            return false;
        }
        component_context__[sock] = sf_component_context_t();
        component_context__[sock].name = name;
        return true;
    }

    inline std::list<std::string> sf_component_server::get_component_list(SOCKET sock)
    {
        std::list<std::string> ret;
        std::shared_lock<std::shared_mutex> lck(mu_private__);
        if(component_context__.count(sock)!=0)
        {
            return ret;
        }
        for(const auto &p:component_context__)
        {
            ret.push_back(p.second.name);
        }
        return ret;
    }

    inline bool sf_component_server::save_private_data(SOCKET sock)
    {
        std::shared_lock<std::shared_mutex> lck(mu_private__);
        if(component_context__.count(sock)!=0)
        {
            return false;
        }
        return sf_write_file(sf_path_join(data_dir__,component_context__[sock].name), sf_serialize_binary(component_context__[sock]),
                             false);
    }


}