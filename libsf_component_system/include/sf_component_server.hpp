#include "sf_component_server.h"

#include "sf_nocopy.hpp"
#include "sf_object.hpp"
#include "sf_type.hpp"
#include "sf_rpcserver.hpp"
#include "sf_utils.hpp"
#include "sf_random.hpp"

#include <algorithm>

namespace skyfire
{

    inline bool sf_component_server::has_component_name__(const std::string &name)
    {
        return std::find_if(component_context__.begin(),
                            component_context__.end(),[=](auto& com){
                    return com.second.name == name;
                }) != component_context__.end();
    }

    inline bool sf_component_server::has_component_token__(const std::string &token)
    {
        return component_context__.count(token) != 0;
    }

    inline std::string sf_component_server::get_token_by_name__(const std::string &name)
    {
        auto ret = std::find_if(component_context__.begin(),
                     component_context__.end(),[=](auto& com){
                    return com.second.name == name;
                });
        if(ret == component_context__.end())
        {
            return "";
        }
        return ret->first;
    }


    inline std::string sf_component_server::reg_component__(std::string name)
    {
        std::unique_lock<std::shared_mutex> lck(mu_private__);
        if(has_component_name__(name))
        {
            return "";
        }
        auto token = sf_random::get_instance()->get_uuid_str();
        component_context__[token] = sf_component_context_t();
        component_context__[token].name = name;
        component_reged(name);
        return token;
    }

    inline std::list<std::string> sf_component_server::get_component_list__(std::string token)
    {
        std::list<std::string> ret;
        if(!has_component_token__(token))
        {
            return ret;
        }
        std::shared_lock<std::shared_mutex> lck(mu_private__);
        for(const auto &p:component_context__)
        {
            ret.push_back(p.first);
        }
        return ret;
    }

    inline bool sf_component_server::save_private_data__(std::string token)
    {
        std::shared_lock<std::shared_mutex> lck(mu_private__);
        if(!has_component_token__(token))
        {
            return false;
        }
        return sf_write_file(sf_path_join(data_dir__,component_context__[token].name),sf_serialize_binary(component_context__[token]),
                             false);
    }

    inline void sf_component_server::load_private_data__(std::string token)
    {
        std::unique_lock<std::shared_mutex> lck(mu_private__);
        if(!has_component_token__(token))
        {
            return;
        }
        byte_array data;
        if(!sf_read_file(sf_path_join(data_dir__,component_context__[token].name),data))
        {
            return;
        }
        sf_deserialize_binary(data,component_context__[token],0);
    }

    inline bool sf_component_server::listen(const std::string &ip, unsigned short port)
    {
        return rpc_server__->listen(ip,port);
    }


    inline std::map<std::string,sf_component_context_t> sf_component_server::get_component_context()
    {
        return component_context__;
    }


    inline void sf_component_server::unreg_component(std::string name)
    {
        std::unique_lock<std::shared_mutex> lck(mu_private__);
        auto token = get_token_by_name__(name);
        if(!token.empty())
            component_context__.erase(token);
    }

    inline void sf_component_server::clear_public_area()
    {
        std::unique_lock<std::shared_mutex> lck(mu_public__);
        public_area__.clear();
    }


}