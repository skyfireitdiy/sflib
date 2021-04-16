#pragma once
#include "http_middleware_logger.h"
#include "logger.h"
#include "utils.h"
namespace skyfire
{
inline bool http_middleware_logger::before(const http_server_request& http_server_request, http_server_response& res)
{
    auto addr = http_server_request.addr();
    sf_info("Request", addr.ip, addr.port,
            http_server_request.request_line().method,
            http_server_request.request_line().http_version,
            http_server_request.request_line().url);
    return true;
}
inline bool http_middleware_logger::after(const http_server_request& http_server_request, http_server_response& res)
{
    auto addr = http_server_request.addr();
    sf_info("Response", res.status(), res.status_desc(),
            addr.ip, addr.port,
            http_server_request.request_line().method,
            http_server_request.request_line().http_version,
            http_server_request.request_line().url);
    return true;
}
}