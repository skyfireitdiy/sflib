#include "sflib/http/http_server_req_header.h"
#include "sflib/http/http_header.h"

namespace skyfire
{
std::unordered_map<std::string, std::string> http_server_req_header::cookies() const
{
    return cookies__;
}
void http_server_req_header::add_cookies(const std::string &key, const std::string &value)
{
    cookies__[key] = value;
}
void http_server_req_header::set_request_line(const http_request_line &request_line)
{
    request_line__ = request_line;
}
}