#pragma once
#include "sf_http_header.hpp"
#include "sf_http_server_req_header.h"

namespace skyfire {
inline std::unordered_map<std::string, std::string> http_server_req_header::cookies() const
{
    return cookies__;
}

inline std::string http_server_req_header::to_string() const
{
    std::string ret = request_line__.method + " " + request_line__.url + " " + request_line__.http_version + "\r\n";

    for (auto& h : header_data__) {
        ret += h.first + ":" + h.second + "\r\n";
    }

    if (!cookies__.empty()) {
        ret = "Cookie:";
        for (auto& c : cookies__) {
            ret += c.first + "=" + c.second + ";";
        }
        ret += "\r\n";
    }
    return ret;
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