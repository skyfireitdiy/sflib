
#include "sflib/http/websocket_router.h"
#include "sflib/http/router.h"

#include "sflib/tools/utils.h"
#include <memory>

namespace skyfire
{
websocket_router::websocket_router(
    const std::string&                            url,
    std::function<void(const websocket_param_t&)> callback, int priority)
    : url__(url)
    , callback__(std::move(callback))
    , priority__(priority)
{
}
websocket_router::websocket_router(
    const std::string& url, void (*callback)(const websocket_param_t&),
    int                priority)
    : url__(url)
    , callback__(std::function(callback))
    , priority__(priority)
{
}
bool websocket_router::run_route(
    const websocket_param_t& context) const
{

    if (context.url == url__)
    {
        callback__(context);
        return true;
    }
    return false;
}
int websocket_router::priority() const { return priority__; }
} // namespace skyfire
