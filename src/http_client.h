#pragma once
#include "http_client_request.h"
#include "nocopy.h"
#include "tcp_client.h"
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