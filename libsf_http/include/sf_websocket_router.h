#pragma once

#include <string>
#include <functional>

#include "sf_router.hpp"
#include "sf_websocket_utils.hpp"
#include "sf_type.hpp"


namespace skyfire {
    class sf_websocket_router : public sf_router {

    private:
        explicit sf_websocket_router(const std::string &url, std::function<void(const websocket_param_t &)> callback);

        explicit sf_websocket_router(const std::string &url, void(*callback)(const websocket_param_t &));

        std::string url__;
        std::function<void(const websocket_param_t &)> callback__;

        bool run_route(const websocket_param_t &context);

        friend sf_http_server;

        friend std::shared_ptr<sf_websocket_router>
        make_websocket_router(const std::string &url, std::function<void(const websocket_param_t &)> callback);

        friend std::shared_ptr<sf_websocket_router>
        make_websocket_router(const std::string &url, void(*callback)(const websocket_param_t &));

    };

}