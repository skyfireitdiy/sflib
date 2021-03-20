
#pragma once
#include "router.h"
namespace skyfire
{
inline int  router::priority() const { return 0; }
inline bool router::run_route(const http_server_request& req,
                              http_server_response& res, const std::string& url,
                              const std::string& method)
{
    return false;
}
} // namespace skyfire
