#pragma once
#include "sflib/http/http_cookie_item.h"
#include "sflib/http/http_header.h"

namespace skyfire
{
class http_server_res_header : public http_header
{
private:
    int                                                 status__       = 200;
    std::string                                         http_version__ = "HTTP/1.1";
    std::string                                         status_desc__  = "OK";
    std::unordered_map<std::string, http_cookie_item_t> res_cookies__;

public:
    void                                                add_cookies(const http_cookie_item_t& cookies);
    void                                                add_cookies(const std::string& key, const std::string& value);
    void                                                clear_cookies(bool clear_session = false);
    void                                                remove_cookies(const std::string& key);
    std::unordered_map<std::string, http_cookie_item_t> cookies() const;
    std::string                                         to_string() const;
    void                                                set_status(int status);
    void                                                set_http_version(const std::string& http_version);
    void                                                set_status_desc(const std::string& desc);
    int                                                 status() const;
    std::string                                         status_desc() const;
    std::string                                         http_version() const;
};
}