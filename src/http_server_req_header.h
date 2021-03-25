#pragma once
#include "http_header.h"
#include "http_request_line.h"
#include "stdc++.h"
namespace skyfire
{
class http_server_req_header : public http_header
{
private:
    std::unordered_map<std::string, std::string> cookies__;
    http_request_line                            request_line__;

public:
    std::unordered_map<std::string, std::string> cookies() const;
    void                                         add_cookies(const std::string& key, const std::string& value);
    void                                         set_request_line(const http_request_line& request_line);
};
}