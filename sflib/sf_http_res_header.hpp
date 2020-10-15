#pragma once

#include "sf_http_header.hpp"
#include "sf_http_res_header.h"

namespace skyfire {

inline void sf_http_res_header::add_cookies(const sf_http_cookie_item_t& cookies)
{
    res_cookies__[cookies.key] = cookies;
}

inline void sf_http_res_header::add_cookies(const std::string& key, const std::string& value)
{
    sf_http_cookie_item_t cookie;
    cookie.key = key;
    cookie.value = value;
    add_cookies(cookie);
}

inline void sf_http_res_header::clear_cookies(bool clear_session)
{
    if (res_cookies__.count(sf_session_id_key) != 0) {
        if (clear_session == false) {
            auto id = res_cookies__[sf_session_id_key];
            res_cookies__.clear();
            res_cookies__[sf_session_id_key] = id;
            return;
        }
    }
    res_cookies__.clear();
}

inline void sf_http_res_header::remove_cookies(const std::string& key)
{
    res_cookies__.erase(key);
}

inline std::unordered_map<std::string, sf_http_cookie_item_t> sf_http_res_header::res_cookies() const
{
    return res_cookies__;
}

inline std::string sf_http_res_header::to_string() const
{
    std::string ret;
    for (auto& p : header_data__) {
        ret += p.first + ":" + p.second + "\r\n";
    }

    for (auto& p : res_cookies__) {
        std::string value_str = p.second.key + "="s + p.second.value + ";"s;
        if (p.second.life_type == cookie_life_type::time_point) {
            value_str += "Expires=" + sf_make_http_time_str(p.second.time_point) + ";"s;
        }
        std::string cookie_path = "/";
        if (!p.second.path.empty()) {
            cookie_path = p.second.path;
        }
        value_str += "Path=" + cookie_path + ";";
        if (p.second.secure) {
            value_str += "Secure;";
        }
        if (p.second.http_only) {
            value_str += "HttpOnly;";
        }
        ret += "Set-Cookie:" + value_str + "\r\n";
    }

    ret += "\r\n";
    return ret;
}

}