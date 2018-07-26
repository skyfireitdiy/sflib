#pragma once

#include <string>
#include <functional>


#include "sf_type.h"


namespace skyfire
{
    class sf_websocket_router{

    public:
        explicit sf_websocket_router(const std::string &url,std::function<bool()>):
            url__(url)
        {}

    private:
        std::string url__;

        friend sf_http_server;
    };
}