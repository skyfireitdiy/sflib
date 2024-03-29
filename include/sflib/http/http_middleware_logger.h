#pragma once
#include "sflib/http/http_middleware.h"
#include "sflib/tools/utils.h"
namespace skyfire
{
class http_middleware_logger : public make_instance_t<http_middleware_logger, http_middleware>
{
    bool before(const http_server_request& http_server_request, http_server_response& res) override;
    bool after(const http_server_request& http_server_request, http_server_response& res) override;
};
}