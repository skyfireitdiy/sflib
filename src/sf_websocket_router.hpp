
#include "sf_stdc++.h"
#include <memory>
#pragma once
#include "sf_utils.h"
#include "sf_websocket_router.h"
namespace skyfire
{
inline websocket_router::websocket_router(
    const std::string&                            url,
    std::function<void(const websocket_param_t&)> callback, int priority)
    : url__(url)
    , callback__(std::move(callback))
    , priority__(priority)
{
}
inline websocket_router::websocket_router(
    const std::string& url, void (*callback)(const websocket_param_t&),
    int                priority)
    : url__(url)
    , callback__(std::function(callback))
    , priority__(priority)
{
}
inline bool websocket_router::run_route(
    const websocket_param_t& context) const
{
    sf_debug("context.url", context.url, "url__", url__);
    if (context.url == url__)
    {
        callback__(context);
        return true;
    }
    return false;
}
inline int websocket_router::priority() const { return priority__; }
} // namespace skyfire
