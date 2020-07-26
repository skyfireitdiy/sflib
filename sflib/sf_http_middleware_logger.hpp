#pragma once

#include "sf_http_middleware_logger.h"
#include "sf_logger.h"
#include "sf_utils.h"

namespace skyfire {
inline bool sf_http_middleware_logger::before(const sf_http_request& http_request, sf_http_response& res)
{
    auto addr = http_request.addr();
    sf_info("Request", addr.ip, addr.port,
        http_request.request_line().method,
        http_request.request_line().http_version,
        http_request.request_line().url);
    return true;
}

inline bool sf_http_middleware_logger::after(const sf_http_request& http_request, sf_http_response& res)
{
    auto addr = http_request.addr();
    sf_info("Response", res.status(), res.status_desc(),
        addr.ip, addr.port,
        http_request.request_line().method,
        http_request.request_line().http_version,
        http_request.request_line().url);
    return true;
}
}