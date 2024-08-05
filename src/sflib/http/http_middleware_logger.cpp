
#include "sflib/http/http_middleware_logger.h"
#include "sflib/tools/utils.h"
namespace skyfire
{
bool http_middleware_logger::before(const http_server_request &http_server_request, http_server_response &res)
{
    auto addr = http_server_request.addr();
    return true;
}
bool http_middleware_logger::after(const http_server_request &http_server_request, http_server_response &res)
{
    auto addr = http_server_request.addr();
    return true;
}
}