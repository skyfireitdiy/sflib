#pragma once

#include <string>
#include <functional>

#include "sf_websocket_utils.h"
#include "sf_type.h"


namespace skyfire
{
    class sf_websocket_router{

    public:
        explicit sf_websocket_router(const std::string &url,std::function<void(const websocket_param_t&)> callback):
                url__(url),callback__(callback)
        {

        }
        explicit sf_websocket_router(const std::string &url,void(*callback)(const websocket_param_t&)):
                url__(url),callback__(std::function(callback))
        {

        }

    private:
        std::string url__;
        std::function<void(const websocket_param_t&)> callback__;

        bool run_route(const websocket_param_t& context){
            if(context.url == url__){
                callback__(context);
                return true;
            }
            return false;
        }

        friend sf_http_server;
    };
}