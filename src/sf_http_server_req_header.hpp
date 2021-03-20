#pragma once
#include "sf_http_header.h"
#include "sf_http_server_req_header.h"
namespace skyfire
{
inline std::unordered_map<std::string, std::string> http_server_req_header::cookies() const
{
    return cookies__;
}
inline void http_server_req_header::add_cookies(const std::string& key, const std::string& value)
{
    cookies__[key] = value;
}
inline void http_server_req_header::set_request_line(const http_request_line& request_line)
{
    request_line__ = request_line;
}
}