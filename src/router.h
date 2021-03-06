
#pragma once
#include "http_server_request.h"
#include "http_server_response.h"
#include "stdc++.h"
namespace skyfire
{
class router
{
public:
    virtual ~router() = default;
    virtual int  priority() const;
    virtual bool run_route(const http_server_request& req, http_server_response& res,
                           const std::string& url, const std::string& method);
};
} // namespace skyfire
