#include "sf_component_server.h"
#include "sf_component_utils.h"
#include "sf_nocopy.hpp"
#include "sf_object.hpp"
#include "sf_type.hpp"
#include "sf_rpcserver.hpp"
#include "sf_utils.hpp"
#include "sf_random.hpp"
#include "sf_msg_bus_server.hpp"

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
        std::unique_lock<std::recursive_mutex> lck(mu_private__);
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
        std::unique_lock<std::recursive_mutex> lck(mu_private__);
        for(const auto &p:component_context__)
        {
            ret.push_back(p.first);
        }
        return ret;
    }

    inline bool sf_component_server::save_private_data__(std::string token)
    {
        std::unique_lock<std::recursive_mutex> lck(mu_private__);
        if(!has_component_token__(token))
        {
            return false;
        }
        return sf_write_file(sf_path_join(data_dir__,component_context__[token].name),sf_serialize_binary(component_context__[token]),
                             false);
    }

    inline void sf_component_server::load_private_data__(std::string token)
    {
        std::unique_lock<std::recursive_mutex> lck(mu_private__);
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


    inline std::map<std::string,sf_component_context_t> sf_component_server::get_component_context()
    {
        return component_context__;
    }


    inline void sf_component_server::unreg_component(std::string name)
    {
        std::unique_lock<std::recursive_mutex> lck(mu_private__);
        auto token = get_token_by_name__(name);
        if(!token.empty())
            component_context__.erase(token);
    }

    inline void sf_component_server::unreg_component__(std::string token)
    {
        if(!has_component_token__(token))
        {
            return;
        }
        auto name = component_context__[token].name;
        unreg_component(name);
        component_unreged(name);
    }

    inline void sf_component_server::clear_public_area()
    {
        std::unique_lock<std::recursive_mutex> lck(mu_public__);
        public_area__.clear();
    }

    inline byte_array sf_component_server::get_public_data(std::string key)
    {
        std::unique_lock<std::recursive_mutex> lck(mu_public__);
        if(public_area__.count(key) == 0)
        {
            return byte_array();
        }
        return public_area__[key];
    }

    inline void sf_component_server::set_public_data(std::string key, byte_array value)
    {
        std::unique_lock<std::recursive_mutex> lck(mu_public__);
        public_area__[key] = value;
    }

    inline void sf_component_server::delete_public_data(std::string key)
    {
        std::unique_lock<std::recursive_mutex> lck(mu_public__);
        public_area__.erase(key);
    }

    inline bool sf_component_server::has_public_data(std::string key)
    {
        return public_area__.count(key) != 0;
    }

    inline bool sf_component_server::save_data()
    {
        for (auto &p:component_context__)
        {
            if (!save_private_data__(p.first))
            {
                return false;
            }
        }
        return save_public_data();
    }

    inline void sf_component_server::load_data()
    {
        for(auto &p:component_context__)
        {
            load_private_data__(p.first);
        }
        load_data();
    }

    inline bool sf_component_server::save_public_data()
    {
        std::unique_lock<std::recursive_mutex> lck(mu_public__);
        using namespace std::string_literals;
        return sf_write_file(sf_path_join(data_dir__,std::string(sf_component_server_shared_data_file_name__)),sf_serialize_binary(public_area__),
                             false);
    }

    inline void sf_component_server::load_public_data()
    {
        std::unique_lock<std::recursive_mutex> lck(mu_public__);
        byte_array data;
        using namespace std::string_literals;
        if(!sf_read_file(sf_path_join(data_dir__,std::string(sf_component_server_shared_data_file_name__)),data))
        {
            return;
        }
        sf_deserialize_binary(data,public_area__,0);
    }


    inline void sf_component_server::clear_public_area__(std::string token)
    {
        if(!has_component_token__(token))
        {
            return;
        }
        clear_public_area();
    }

    inline byte_array sf_component_server::get_public_data__(std::string token, std::string key)
    {
        if(!has_component_token__(token))
        {
            return byte_array();
        }
        return get_public_data(key);
    }

    inline void sf_component_server::set_public_data__(std::string token, std::string key, byte_array value)
    {
        if(!has_component_token__(token))
        {
            return;
        }
        set_public_data(key,value);
    }

    inline void sf_component_server::delete_public_data__(std::string token, std::string key)
    {
        if(!has_component_token__(token))
        {
            return;
        }
        delete_public_data(key);
    }

    inline bool sf_component_server::has_public_data__(std::string token, std::string key)
    {
        if(!has_component_token__(token))
        {
            return false;
        }
        return has_public_data(key);
    }


    inline void sf_component_server::clear_private_area__(std::string token)
    {
        if(!has_component_token__(token))
        {
            return ;
        }
        std::unique_lock<std::recursive_mutex> lck(mu_private__);
        component_context__[token].private_data.clear();
    }

    inline byte_array sf_component_server::get_private_data__(std::string token,std::string key)
    {
        if(!has_component_token__(token))
        {
            return byte_array();
        }
        std::unique_lock<std::recursive_mutex> lck(mu_private__);
        if(component_context__[token].private_data.count(key) ==0)
        {
            return byte_array();
        }
        return component_context__[token].private_data[key];
    }

    inline void sf_component_server::set_private_data__(std::string token,std::string key, byte_array value)
    {
        if(!has_component_token__(token))
        {
            return ;
        }
        std::unique_lock<std::recursive_mutex> lck(mu_private__);
        component_context__[token].private_data[key] = value;
    }

    inline void sf_component_server::delete_private_data__(std::string token,std::string key)
    {
        if(!has_component_token__(token))
        {
            return ;
        }
        std::unique_lock<std::recursive_mutex> lck(mu_private__);
        component_context__[token].private_data.erase(key);
    }

    inline bool sf_component_server::has_private_data__(std::string token,std::string key)
    {
        if(!has_component_token__(token))
        {
            return false;
        }
        std::unique_lock<std::recursive_mutex> lck(mu_private__);
        return component_context__[token].private_data.count(key) !=0;
    }


    inline sf_component_server::sf_component_server(const std::string &data_dir):
        data_dir__(data_dir)
    {
        rpc_server__->reg_rpc_func("reg_component", [=](std::string name){ return reg_component__(name);});
        rpc_server__->reg_rpc_func("get_component_list",[=](std::string token){ return get_component_list__(token);});
        rpc_server__->reg_rpc_func("clear_private_area",[=](std::string token){ clear_private_area__(token);});
        rpc_server__->reg_rpc_func("get_private_data",[=](std::string token,std::string key){ return get_private_data__(token,key);});
        rpc_server__->reg_rpc_func("set_private_data",[=](std::string token,std::string key,byte_array value){ set_private_data__(token,key,value);});
        rpc_server__->reg_rpc_func("delete_private_data",[=](std::string token,std::string key){ delete_private_data__(token,key);});
        rpc_server__->reg_rpc_func("has_private_data",[=](std::string token,std::string key){ return has_private_data__(token,key);});
        rpc_server__->reg_rpc_func("save_private_data",[=](std::string token){ return save_private_data__(token);});
        rpc_server__->reg_rpc_func("load_private_data",[=](std::string token){ load_private_data__(token);});
        rpc_server__->reg_rpc_func("unreg_component",[=](std::string token){ unreg_component__(token);});
        //rpc_server__->reg_rpc_func("clear_public_area",[=](std::string token){ clear_public_area__(token);});
        rpc_server__->reg_rpc_func("get_public_data",[=](std::string token,std::string key){ return get_public_data__(token,key);});
        rpc_server__->reg_rpc_func("set_public_data",[=](std::string token,std::string key,byte_array value){ set_public_data__(token,key,value);});
        rpc_server__->reg_rpc_func("delete_public_data",[=](std::string token){ unreg_component__(token);});
        rpc_server__->reg_rpc_func("has_public_data",[=](std::string token,std::string key){ return has_public_data__(token,key);});
        rpc_server__->reg_rpc_func("get_msg_bus_server_addr",[=](std::string token){ return get_msg_bus_server_addr__(token);});
    }

    inline bool sf_component_server::listen(const std::string& ip, unsigned short port)
    {
        if(!rpc_server__->listen(ip,port))
            return false;
        if(!msg_bus_server__->listen(ip,0))
        {
            rpc_server__->close();
            return false;
        }
        return true;
    }

    inline addr_info_t sf_component_server::get_msg_bus_server_addr()
    {
        addr_info_t ret;
        memset(&ret,0, sizeof(ret));
        msg_bus_server__->get_server_addr(ret);
        return ret;
    }

    inline addr_info_t sf_component_server::get_msg_bus_server_addr__(std::string token)
    {
        if(!has_component_token__(token))
        {
            return addr_info_t{0};
        }
        return get_msg_bus_server_addr();
    }

    inline void sf_component_server::send_msg(const std::string &type, const byte_array &data)
    {
        msg_bus_server__->send_msg(type, data);
    }

    inline std::shared_ptr<sf_component_server> sf_component_server::make_server(const std::string &data_dir)
    {
        return std::shared_ptr<sf_component_server>(new sf_component_server(data_dir));
    }
}