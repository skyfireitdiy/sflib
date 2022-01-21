#include "sflib/http_server_res_header.h"
#include "sflib/http_header.h"
#include "sflib/http_session_id_key.h"
#include "sflib/http_utils.h"

namespace skyfire
{
void http_server_res_header::add_cookies(const http_cookie_item_t& cookies)
{
    res_cookies__[cookies.key] = cookies;
}
void http_server_res_header::add_cookies(const std::string& key, const std::string& value)
{
    http_cookie_item_t cookie;
    cookie.key   = key;
    cookie.value = value;
    add_cookies(cookie);
}
void http_server_res_header::clear_cookies(bool clear_session)
{
    if (res_cookies__.count(session_id_key) != 0)
    {
        if (clear_session == false)
        {
            auto id = res_cookies__[session_id_key];
            res_cookies__.clear();
            res_cookies__[session_id_key] = id;
            return;
        }
    }
    res_cookies__.clear();
}
void http_server_res_header::remove_cookies(const std::string& key)
{
    res_cookies__.erase(key);
}
std::unordered_map<std::string, http_cookie_item_t> http_server_res_header::cookies() const
{
    return res_cookies__;
}
std::string http_server_res_header::to_string() const
{
    std::string ret = http_version__ + " " + std::to_string(status__) + " " + status_desc__ + "\r\n";
    for (auto& p : header_data__)
    {
        ret += p.first + ":" + p.second + "\r\n";
    }
    for (auto& p : res_cookies__)
    {
        using namespace std::literals;
        std::string value_str = p.second.key + "="s + p.second.value + ";"s;
        if (p.second.life_type == cookie_life_type::time_point)
        {
            value_str += "Expires=" + make_http_time_str(p.second.time_point) + ";"s;
        }
        std::string cookie_path = "/";
        if (!p.second.path.empty())
        {
            cookie_path = p.second.path;
        }
        value_str += "Path=" + cookie_path + ";";
        if (p.second.secure)
        {
            value_str += "Secure;";
        }
        if (p.second.http_only)
        {
            value_str += "HttpOnly;";
        }
        ret += "Set-Cookie:" + value_str + "\r\n";
    }
    ret += "\r\n";
    return ret;
}
void http_server_res_header::set_status(int status)
{
    status__ = status;
}
void http_server_res_header::set_http_version(
    const std::string& http_version)
{
    http_version__ = http_version;
}
void http_server_res_header::set_status_desc(const std::string& desc)
{
    status_desc__ = desc;
}
int http_server_res_header::status() const
{
    return status__;
}
std::string http_server_res_header::status_desc() const
{
    return status_desc__;
}
std::string http_server_res_header::http_version() const
{
    return http_version__;
}
}