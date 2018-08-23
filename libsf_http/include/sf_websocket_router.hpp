#pragma once

#include "sf_websocket_router.h"

namespace skyfire {

    inline std::shared_ptr<sf_websocket_router> make_websocket_router(const std::string &url,
                                                                      std::function<void(
                                                                              const websocket_param_t &)> callback) {
        return std::shared_ptr<sf_websocket_router>(new sf_websocket_router(url, callback));
    }

    inline std::shared_ptr<sf_websocket_router>
    make_websocket_router(const std::string &url, void(*callback)(const websocket_param_t &)) {
        return std::shared_ptr<sf_websocket_router>(new sf_websocket_router(url, callback));
    }

    inline sf_websocket_router::sf_websocket_router(const std::string &url,
                                             std::function<void(const websocket_param_t &)> callback) :
            url__(url), callback__(callback) {

    }

    inline sf_websocket_router::sf_websocket_router(const std::string &url, void (*callback)(const websocket_param_t &)) :
            url__(url), callback__(std::function(callback)) {

    }

    inline bool sf_websocket_router::run_route(const websocket_param_t &context) {
        if (context.url == url__) {
            callback__(context);
            return true;
        }
        return false;
    }

}