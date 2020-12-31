
#pragma once
#include "sf_http_router.h"
#include "sf_http_server_request.h"
#include "sf_http_server_response.h"
#include "sf_router.hpp"
#include "sf_router_shared_ptr_compare.h"
#include "sf_stdc++.h"
#include "sf_utils.h"
namespace skyfire
{
class http_part_router final
    : public make_instance_t<http_part_router, router>
{
private:
    const int                                                              priority__ = 10;
    const std::vector<std::string>                                         methods__;
    std::recursive_mutex                                                   methods_mu__;
    std::vector<std::string>                                               prefix__;
    std::function<bool(const http_server_request&, http_server_response&)> callback__;
    std::multiset<std::shared_ptr<router>,
                  router_shared_ptr_compare__<router>>
        endpoint_router__;
    std::multiset<std::shared_ptr<router>,
                  router_shared_ptr_compare__<router>>
        middle_router__;

public:
    http_part_router(
        std::string prefix,
        std::function<bool(const http_server_request&, http_server_response&)>
                                 callback,
        std::vector<std::string> methods  = { { "*" } },
        int                      priority = default_http_part_priority);
    http_part_router(const std::string& prefix,
                     bool (*callback)(const http_server_request&,
                                      http_server_response&),
                     const std::vector<std::string>& methods  = { { "*" } },
                     int                             priority = default_http_part_priority);
    void              add_router(std::shared_ptr<router> router);
    void              add_router(std::shared_ptr<http_part_router> router);
    bool              run_route(const http_server_request& req, http_server_response& res,
                                const std::string& url, const std::string& method) override;
    bool              operator<(const http_part_router& router) const;
    [[nodiscard]] int priority() const override;
};
} // namespace skyfire
