#pragma once

#include "sf_nocopy.h"
#include "sf_object.h"
#include "sf_type.h"
#include "sf_rpcserver.h"
#include "sf_component_context.h"

#include <memory>
#include <string>
#include <functional>

namespace skyfire
{
    class sf_component_server : public sf_nocopy<sf_object>
    {
        // 组件注册信号
        SF_REG_SIGNAL(component_reg, SOCKET, const std::string&);
        // 组件反注册信号
        SF_REG_SIGNAL(component_unreg, SOCKET, const std::string&);

    private:
        std::map<std::string, byte_array> shared_area__;
        std::shared_ptr<sf_rpcserver> rpc_server__ = sf_rpcserver::make_server();

        byte_array get_private_data(SOCKET sock, std::string key) const;
        void delete_private_data(SOCKET sock, std::string key) ;
        void clear_private_data(SOCKET sock) ;
        void set_private_data(SOCKET sock,std::string key, byte_array value);
        bool has_private_data(SOCKET sock,std::string key) const;

        void reg_component(SOCKET sock, std::string name);

    public:

        sf_component_server();

        bool listen(const std::string& ip, unsigned short port);
        std::list<sf_component_context_t> get_component_context();
        void std::unreg_component(std::string name);

        void clear_public_area();
        byte_array get_public_data(std::string key);
        void set_public_data(std::string key, byte_array value);
        void delete_public_data(std::string key);
        bool has_public_data(std::string key) const;
    };
}