#pragma once

#include "sf_http_client_request.h"

namespace skyfire {

inline std::shared_ptr<http_client_request> http_client_request::set_body(const byte_array& body)
{
    body__ = body;
    type__ = http_data_type ::normal;
}

inline std::shared_ptr<http_client_request> http_client_request::add_header(const std::string& key, const std::string& value)
{
    header__.set_header(key, value);
}

inline std::shared_ptr<http_client_request> http_client_request::add_cookies(const std::string& key, const std::string& value)
{
    header__.add_cookies(key, value);
}

inline std::shared_ptr<http_client_request> http_client_request::set_url(const std::string& url)
{
    header__.get_request_line().url = url;
}

inline std::shared_ptr<http_client_request> http_client_request::set_method(const std::string& method)
{
    header__.get_request_line().method = method;
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
}

std::shared_ptr<http_client_request> http_client_request::set_stream(std::shared_ptr<std::istream> stream)
{
    stream__ = stream;
    type__ = http_data_type::stream;
}

std::shared_ptr<http_client_request> http_client_request::add_multipart(const http_multipart_info_t& multip)
{
    multipart_info_vec__.push_back(multip);
    type__ = http_data_type ::multipart;
}

std::shared_ptr<http_request_result> http_client_request::do(std::shared_ptr<http_client> client)
{
    // TODO 发送http请求
    return nullptr;
}
}