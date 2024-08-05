#pragma once
#include "sflib/http/http_header.h"
#include "sflib/http/http_request_line.h"
namespace skyfire
{
class http_client_req_header : public http_header
{
private:
    std::unordered_map<std::string, std::string> cookies__;
    http_request_line                            request_line__;

public:
    std::string              to_string() const;
    void                     add_cookies(const std::string& key, const std::string& value);
    void                     set_request_line(const http_request_line& request_line);
    const http_request_line& get_request_line() const;
    http_request_line&       get_request_line();
};
}