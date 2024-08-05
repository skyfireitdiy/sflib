
#include "sflib/http/http_router.h"
#include "sflib/http/router.h"

#include "sflib/tools/utils.h"
#include <utility>
namespace skyfire
{

bool http_router::run_route(const http_server_request &req,
                            http_server_response &res,
                            const std::string &url,
                            const std::string &method)
{
    {

        std::unique_lock lck(methods_mu__);
        using namespace std::literals;
        if (methods__.cend() == std::find(methods__.cbegin(), methods__.cend(), "*" s))
        {
            if (methods__.cend() == std::find(methods__.cbegin(), methods__.cend(), method))
            {

                return false;
            }
        }
    }
    return route_callback__(req, res, url);
}
int http_router::priority() const
{
    return priority__;
}
bool http_router::operator<(const http_router &router) const
{
    return priority__ < router.priority__;
}
} // namespace skyfire