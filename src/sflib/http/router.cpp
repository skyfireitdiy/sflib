
#include "sflib/http/router.h"
namespace skyfire
{
int router::priority() const
{
    return 0;
}
bool router::run_route(const http_server_request &req,
                       http_server_response &res, const std::string &url,
                       const std::string &method)
{
    return false;
}
} // namespace skyfire
