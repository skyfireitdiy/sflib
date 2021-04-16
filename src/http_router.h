
#pragma once
#include "http_server_request.hpp"
#include "http_server_response.hpp"
#include "router.hpp"
#include "stdc++.h"
#include "utils.h"
namespace skyfire
{
class http_router final
    : public make_instance_t<http_router, router>
{
private:
    std::function<bool(const http_server_request&, http_server_response&,
                       const std::string&)>
                                   route_callback__;
    const int                      priority__ {};
    const std::vector<std::string> methods__;
    std::recursive_mutex           methods_mu__;
    template <typename FuncType, int N, typename... Args>
    static typename std::enable_if<sizeof...(Args) == N, void>::type
    callback_call_helper__(const http_server_request& req, http_server_response& res,
                           FuncType func, const std::smatch& sm, Args... args);
    template <typename FuncType, int N, typename... Args>
    static typename std::enable_if<sizeof...(Args) != N, void>::type
    callback_call_helper__(const http_server_request& req, http_server_response& res,
                           FuncType func, const std::smatch& sm, Args... args);
    ;

public:
    template <typename... StringType>
    http_router(const std::string& pattern,
                void (*callback)(const http_server_request&, http_server_response&,
                                 StringType...),
                const std::vector<std::string>& methods  = { { "*" } },
                int                             priority = default_http_endpoint_priority);
    template <typename... StringType>
    http_router(const std::string& pattern,
                std::function<void(const http_server_request&,
                                   http_server_response&, StringType...)>
                                         callback,
                std::vector<std::string> methods  = { { "*" } },
                int                      priority = default_http_endpoint_priority);
    bool run_route(const http_server_request& req, http_server_response& res,
                   const std::string& url, const std::string& method) override;
    bool operator<(const http_router& router) const;
    int  priority() const override;
};
} // namespace skyfire
