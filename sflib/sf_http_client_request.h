
#pragma once

#include "sf_http_client_response.h"
#include "sf_http_req_cookie.h"
#include "sf_http_client_req_header.h"
#include "sf_http_utils.h"
#include "sf_nocopy.h"
#include "sf_stdc++.h"
#include "ssf_http_client.h"
#include "sf_http_request_line.h"

using http_request_result = multi_value<err, http_client_response>;

namespace skyfire {
class http_client_request : public nocopy<make_instance_t<http_client_request>> {
private:
    byte_array body__;
    http_client_req_header header__;
    std::shared_ptr<std::istream> stream__;

    http_data_type type__ = { http_data_type ::normal };
    http_file_info_t file_info__;
    std::vector<http_multipart_info_t> multipart_info_vec__;

public:
    std::shared_ptr<http_client_request> set_body(const byte_array& data);
    std::shared_ptr<http_client_request> add_header(const std::string& key, const std::string& value);
    std::shared_ptr<http_client_request> add_cookies(const std::string& key, const std::string& value);
    std::shared_ptr<http_client_request> set_url(const std::string& url);
    std::shared_ptr<http_client_request> set_method(const std::string& method);
    std::shared_ptr<http_client_request> set_file(const std::string& filename);
    std::shared_ptr<http_client_request> set_stream(std::shared_ptr<std::istream> stream);
    std::shared_ptr<http_client_request> add_multipart(const http_multipart_info_t& multip);

    std::shared_ptr<http_request_result> do(std::shared_ptr<http_client> client);
};
}