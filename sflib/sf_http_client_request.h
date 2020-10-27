
#pragma once

#include "sf_http_client_response.h"
#include "sf_http_req_cookie.h"
#include "sf_http_req_header.h"
#include "sf_nocopy.h"
#include "sf_stdc++.h"
#include "ssf_http_client.h"

using http_request_result = multi_value<err, http_client_response>;

namespace skyfire {
class http_client_request : public nocopy<> {
private:
public:
    std::shared_ptr<http_client_request> body(const byte_array& data);
    std::shared_ptr<http_client_request> header(const http_req_header& header);
    std::shared_ptr<http_client_request> cookies(const std::unordered_map<std::string, std::string>& cookies);

    std::shared_ptr<http_request_result> do(std::shared_ptr<http_client> client);
};
}