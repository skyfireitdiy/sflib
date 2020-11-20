#pragma once

#include "sf_http_client_request.h"
#include "sf_nocopy.h"
#include "sf_tcp_client.h"

namespace skyfire
{

using http_request_result = multi_value<err, http_client_response>;
class http_client : public nocopy<>
{
private:
    std::shared_ptr<tcp_client> tcp_client__ = tcp_client::make_instance();

public:
    std::shared_ptr<http_request_result> request(std::shared_ptr<http_client_request>& req);
};
}