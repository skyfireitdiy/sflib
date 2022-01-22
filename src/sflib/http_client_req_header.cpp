
#include "sflib/http/http_client_req_header.h"
#include "sflib/http/http_header.h"
#include "sflib/tools/utils.h"
namespace skyfire
{
std::string http_client_req_header::to_string() const
{
    std::string ret = request_line__.method + " " + request_line__.url + " " + request_line__.http_version + "\r\n";
    for (auto& h : header_data__)
    {
        ret += h.first + ":" + h.second + "\r\n";
    }
    if (!cookies__.empty())
    {
        ret = "Cookie:";
        for (auto& c : cookies__)
        {
            ret += c.first + "=" + c.second + ";";
        }
        ret += "\r\n";
    }
    ret += "\r\n";
    return ret;
}
void http_client_req_header::add_cookies(const std::string& key, const std::string& value)
{
    cookies__[key] = value;
}
void http_client_req_header::set_request_line(const http_request_line& request_line)
{
    request_line__ = request_line;
}
const http_request_line& http_client_req_header::get_request_line() const
{
    return request_line__;
}
http_request_line& http_client_req_header::get_request_line()
{
    return request_line__;
}
}