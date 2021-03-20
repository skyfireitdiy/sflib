
#pragma once
#include "sf_http_server_request.h"
#include "sf_http_server_res_header.h"
#include "sf_http_server_response.h"
#include "sf_http_status.h"
#include "sf_json.h"
namespace skyfire
{
inline http_server_response::http_server_response(const http_server_request& request)
    : req__(request)
{
}
inline int http_server_response::status() const
{
    return header__.status();
}
inline std::string http_server_response::status_desc() const
{
    return header__.status_desc();
}
inline std::string http_server_response::http_version() const
{
    return header__.http_version();
}
inline const http_server_request http_server_response::get_req() const { return req__; }
inline void                      http_server_response::set_status(int status)
{
    header__.set_status(status);
    if (http_status.count(status) != 0)
    {
        set_status_desc(http_status[status]);
        set_body(to_byte_array(http_status[status]));
    }
    else
    {
        using namespace std::literals;
        set_status_desc("Unknown");
        set_body(to_byte_array("Unknown"s));
    }
}
inline void http_server_response::set_http_version(
    const std::string& http_version)
{
    header__.set_http_version(http_version);
}
inline void http_server_response::set_status_desc(const std::string& desc)
{
    header__.set_status_desc(desc);
}
inline void http_server_response::set_header(const http_server_res_header& header)
{
    header__ = header;
}
inline void http_server_response::set_body(const byte_array& body)
{
    type__ = http_data_type ::normal;
    body__ = body;
}
inline void http_server_response::set_json(const json& json)
{
    type__ = http_data_type ::normal;
    body__ = to_byte_array(json.to_string());
    header__.set_header("Content-Type", "application/json");
}
inline void http_server_response::set_file(
    const http_file_info_t& file_info)
{
    type__      = http_data_type ::file;
    file_info__ = file_info;
}
inline void http_server_response::set_file(const std::string& filename)
{
    type__      = http_data_type ::file;
    file_info__ = http_file_info_t { filename, 0, -1 };
}
inline void http_server_response::set_multipart(
    const std::vector<http_multipart_info_t>& multipart_info_vec)
{
    type__               = http_data_type ::multipart;
    multipart_info_vec__ = multipart_info_vec;
}
inline unsigned long long http_server_response::length() const
{
    return body__.size();
}
inline http_header& http_server_response::header() { return header__; }
inline byte_array   http_server_response::to_package() const
{
    if (type__ != http_data_type::normal)
        return byte_array();
    auto pkg = to_header_package();
    pkg.insert(pkg.end(), body__.begin(), body__.end());
    return pkg;
}
inline byte_array http_server_response::to_header_package() const
{
    std::string response_head;
    response_head += header__.to_string();
    return { response_head.begin(), response_head.end() };
}
inline http_data_type http_server_response::type() const
{
    return type__;
}
inline http_file_info_t http_server_response::file() const
{
    return file_info__;
}
inline std::vector<http_multipart_info_t>
http_server_response::multipart() const
{
    return multipart_info_vec__;
}
inline void http_server_response::add_cookie(const http_cookie_item_t& cookie_data)
{
    header__.add_cookies(cookie_data);
}
inline void http_server_response::remove_cookie(const std::string& key)
{
    header__.remove_cookies(key);
}
inline std::unordered_map<std::string, http_cookie_item_t>
http_server_response::cookies() const
{
    return header__.cookies();
}
inline void http_server_response::redirect(const std::string& new_location,
                                           int                code)
{
    set_status(code);
    header().set_header("Location", new_location);
}
inline void http_server_response::add_header(const std::string& key,
                                             const std::string& value)
{
    header().set_header(key, value);
}
inline void http_server_response::add_cookie(const std::string& key,
                                             const std::string& value)
{
    header__.add_cookies(key, value);
}
inline void http_server_response::clear_cookie(bool clear_session)
{
    header__.clear_cookies(clear_session);
}
inline void http_server_response::set_text(const std::string& text)
{
    body__ = to_byte_array(text);
    add_header("Content-Type", "text/plain");
}
inline void http_server_response::set_html(const std::string& html)
{
    body__ = to_byte_array(html);
    add_header("Content-Type", "text/html");
}
inline void http_server_response::set_content_type(
    const std::string& content_type)
{
    add_header("Content-Type", content_type);
}
} // namespace skyfire
