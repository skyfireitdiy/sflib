#pragma once
#include "http_middleware_logger.h"

#include "utils.h"
namespace skyfire
{
inline bool http_middleware_logger::before(const http_server_request& http_server_request, http_server_response& res)
{
    auto addr = http_server_request.addr();
    return true;
}
inline bool http_middleware_logger::after(const http_server_request& http_server_request, http_server_response& res)
{
    auto addr = http_server_request.addr();
    return true;
}
}