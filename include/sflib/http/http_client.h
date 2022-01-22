#pragma once
#include "sflib/http/http_client_request.h"
#include "sflib/tcp/tcp_client.h"
#include "sflib/tools/nocopy.h"


namespace skyfire
{
using http_request_result = multi_value<sf_error, http_client_response>;
class http_client : public nocopy<>
{
private:
    std::shared_ptr<tcp_client> tcp_client__ = tcp_client::make_instance();

public:
    std::shared_ptr<http_request_result> request(std::shared_ptr<http_client_request>& req);
};
}