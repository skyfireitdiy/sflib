
#pragma once
#include "sflib/http/http_server_request.h"
#include "sflib/http/http_server_response.h"
#include "sflib/http/router.h"
#include "sflib/http/router_shared_ptr_compare.h"
#include "sflib/tools/utils.h"

namespace skyfire
{
class http_part_router final
    : public make_instance_t<http_part_router, router>
{
private:
    const int priority__ = 10;
    const std::vector<std::string> methods__;
    std::recursive_mutex methods_mu__;
    std::vector<std::string> prefix__;
    std::function<bool(const http_server_request &, http_server_response &)> callback__;
    std::multiset<std::shared_ptr<router>,
                  router_shared_ptr_compare__<router>>
        endpoint_router__;
    std::multiset<std::shared_ptr<router>,
                  router_shared_ptr_compare__<router>>
        middle_router__;

public:
    http_part_router(
        std::string prefix,
        std::function<bool(const http_server_request &, http_server_response &)>
            callback,
        std::vector<std::string> methods = {{"*"}},
        int priority = default_http_part_priority);
    void add_router(std::shared_ptr<router> router);
    void add_router(std::shared_ptr<http_part_router> router);
    bool run_route(const http_server_request &req, http_server_response &res,
                   const std::string &url, const std::string &method) override;
    bool operator<(const http_part_router &router) const;
    [[nodiscard]] int priority() const override;
};
} // namespace skyfire
