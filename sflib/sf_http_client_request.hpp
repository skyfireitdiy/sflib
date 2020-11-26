#pragma once

#include "sf_http_client.hpp"
#include "sf_http_client_req_header.hpp"
#include "sf_http_client_request.h"

namespace skyfire
{

inline std::shared_ptr<http_client_request> http_client_request::set_body(const byte_array& body)
{
    body__ = body;
    type__ = http_data_type ::normal;
    return shared_from_this();
}

inline std::shared_ptr<http_client_request> http_client_request::add_header(const std::string& key, const std::string& value)
{
    header__.set_header(key, value);
    return shared_from_this();
}

inline std::shared_ptr<http_client_request> http_client_request::add_cookies(const std::string& key, const std::string& value)
{
    header__.add_cookies(key, value);
    return shared_from_this();
}

inline std::shared_ptr<http_client_request> http_client_request::set_url(const std::string& url)
{
    // 忽略agreement
    std::string agreement;
    parse_client_req_url(url, agreement, server_host__, server_port__, header__.get_request_line().url);
    return shared_from_this();
}

inline std::shared_ptr<http_client_request> http_client_request::set_method(const std::string& method)
{
    header__.get_request_line().method = method;
    return shared_from_this();
}

inline std::shared_ptr<http_client_request> http_client_request::set_file(const std::string& filename)
{
    file_info__ = http_file_info_t {
        filename,
        0,
        -1,
        fs::file_size(filename)
    };
    type__ = http_data_type::file;
    return shared_from_this();
}

std::shared_ptr<http_client_request> http_client_request::set_stream(std::shared_ptr<std::istream> stream)
{
    stream__ = stream;
    type__   = http_data_type::stream;
    return shared_from_this();
}

std::shared_ptr<http_client_request> http_client_request::add_multipart(const http_multipart_info_t& multip)
{
    multipart_info_vec__.push_back(multip);
    type__ = http_data_type ::multipart;
    return shared_from_this();
}

}