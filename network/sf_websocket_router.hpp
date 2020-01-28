
/**
* @version 1.0.0
* @author skyfire
* @file sf_websocket_router.hpp

* 
*/

#include <memory>

/**
* @author skyfire
* @file sf_websocket_router.hpp
*/

#include "core/sf_stdc++.h"

#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCUnusedGlobalDeclarationInspection"
#pragma once

#include "sf_websocket_router.h"
#include "tools/sf_utils.hpp"

namespace skyfire {

inline sf_websocket_router::sf_websocket_router(
    const std::string &url,
    std::function<void(const sf_websocket_param_t &)> callback, int priority)
    : url__(url), callback__(std::move(callback)), priority__(priority) {}

inline sf_websocket_router::sf_websocket_router(
    const std::string &url, void (*callback)(const sf_websocket_param_t &),
    int priority)
    : url__(url), callback__(std::function(callback)), priority__(priority) {}

inline bool sf_websocket_router::run_route(
    const sf_websocket_param_t &context) const {
    sf_debug("context.url", context.url, "url__", url__);
    if (context.url == url__) {
        callback__(context);
        return true;
    }
    return false;
}

}    // namespace skyfire
#pragma clang diagnostic pop