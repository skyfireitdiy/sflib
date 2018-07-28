#pragma once

#include <string>
#include <functional>

#include "sf_websocket_utils.h"
#include "sf_type.h"


namespace skyfire
{
    class sf_websocket_router{

    public:
        explicit sf_websocket_router(const std::string &url,std::function<bool(const websocket_context_t&, 
        std::shared_ptr<sf_http_server>)> callback):
            url__(url),callback__(callback)
        {

        }

    private:
        std::string url__;
        std::function<bool(const websocket_context_t&, std::shared_ptr<sf_http_server>)> callback__;

        bool run_route(std::shared_ptr<sf_http_server> server, const websocket_context_t& context){
            if(context.url == url__){
                callback__(context, server);
                return true;
            }
            return false;
        }

        friend sf_http_server;
    };
}