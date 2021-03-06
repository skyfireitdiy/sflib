#pragma once
#include "http_server_request.h"
#include "http_server_response.h"
namespace skyfire
{
class http_middleware
{
public:
    virtual bool before(const http_server_request& http_server_request, http_server_response& res) = 0;
    virtual bool after(const http_server_request& http_server_request, http_server_response& res)  = 0;
};
}