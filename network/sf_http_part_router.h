
#pragma once
#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCUnusedGlobalDeclarationInspection"

#include "core/sf_stdc++.h"
#include "sf_http_request.hpp"
#include "sf_http_response.hpp"
#include "sf_http_router.h"
#include "sf_router.hpp"
#include "sf_router_shared_ptr_compare.h"
#include "tools/sf_utils.h"

namespace skyfire {
/**
 * @brief  http路由
 */
class sf_http_part_router final
    : public sf_make_instance_t<sf_http_part_router, sf_router> {
   private:
    const int priority__ = 10;
    const std::vector<std::string> methods__;
    std::recursive_mutex methods_mu__;
    std::vector<std::string> prefix__;
    std::function<bool(const sf_http_request &, sf_http_response &)> callback__;

    std::multiset<std::shared_ptr<sf_router>,
                  sf_router_shared_ptr_compare__<sf_router>>
        endpoint_router__;
    std::multiset<std::shared_ptr<sf_router>,
                  sf_router_shared_ptr_compare__<sf_router>>
        middle_router__;

   public:
    template <typename... StringType>
    sf_http_part_router(
        std::string prefix,
        std::function<bool(const sf_http_request &, sf_http_response &)>
            callback,
        std::vector<std::string> methods = {{"*"}},
        int priority = default_http_part_priority);

    template <typename... StringType>
    sf_http_part_router(const std::string &prefix,
                        bool (*callback)(const sf_http_request &,
                                         sf_http_response &),
                        const std::vector<std::string> &methods = {{"*"}},
                        int priority = default_http_part_priority);

    void add_router(std::shared_ptr<sf_router> router);

    void add_router(std::shared_ptr<sf_http_part_router> router);

    bool run_route(const sf_http_request &req, sf_http_response &res,
                   const std::string &url, const std::string &method) override;

    bool operator<(const sf_http_part_router &router) const;

    [[nodiscard]] int get_priority() const override;
};
}    // namespace skyfire
#pragma clang diagnostic pop