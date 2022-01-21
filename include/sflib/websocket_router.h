
#pragma once
#include "router.h"
#include "stdc++.h"
#include "type.h"
#include "utils.h"
#include "websocket_utils.h"
namespace skyfire
{
class websocket_router
    : public make_instance_t<websocket_router, router>
{
private:
    std::string                                   url__;
    std::function<void(const websocket_param_t&)> callback__;
    const int                                     priority__;

public:
    explicit websocket_router(
        const std::string&                            url,
        std::function<void(const websocket_param_t&)> callback,
        int                                           priority = 0);
    explicit websocket_router(const std::string& url,
                              void (*callback)(const websocket_param_t&),
                              int priority = 0);
    int  priority() const override;
    bool run_route(const websocket_param_t& context) const;
    bool run_route(const http_server_request& req,
                   http_server_response& res, const std::string& url,
                   const std::string& method) override
    {
        return false;
    }
};
} // namespace skyfire
