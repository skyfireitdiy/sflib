#pragma once

#include <string>
#include <functional>

#include "sf_router.h"
#include "sf_websocket_utils.h"
#include "sf_type.h"


namespace skyfire {
    class sf_websocket_router : public sf_router {

    private:
        explicit sf_websocket_router(const std::string &url, std::function<void(const websocket_param_t &)> callback) :
                url__(url), callback__(callback) {

        }

        explicit sf_websocket_router(const std::string &url, void(*callback)(const websocket_param_t &)) :
                url__(url), callback__(std::function(callback)) {

        }

        std::string url__;
        std::function<void(const websocket_param_t &)> callback__;

        bool run_route(const websocket_param_t &context) {
            if (context.url == url__) {
                callback__(context);
                return true;
            }
            return false;
        }

        friend sf_http_server;

        friend std::shared_ptr<sf_websocket_router>
        make_websocket_router(const std::string &url, std::function<void(const websocket_param_t &)> callback);

        friend std::shared_ptr<sf_websocket_router>
        make_websocket_router(const std::string &url, void(*callback)(const websocket_param_t &));

    };


    inline std::shared_ptr<sf_websocket_router> make_websocket_router(const std::string &url,
                                                                      std::function<void(
                                                                              const websocket_param_t &)> callback) {
        return std::shared_ptr<sf_websocket_router>(new sf_websocket_router(url, callback));
    }

    inline std::shared_ptr<sf_websocket_router>
    make_websocket_router(const std::string &url, void(*callback)(const websocket_param_t &)) {
        return std::shared_ptr<sf_websocket_router>(new sf_websocket_router(url, callback));
    }

}