
#pragma once
#include "sflib/http/http_server_request.h"
#include "sflib/http/http_server_response.h"
#include "sflib/http/router.h"
#include "sflib/tools/utils.h"
#include <regex>
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

template <typename FuncType, int N, typename... Args>
typename std::enable_if<sizeof...(Args) == N, void>::type
http_router::callback_call_helper__(const http_server_request& req,
                                    http_server_response& res, FuncType func,
                                    const std::smatch& sm, Args... args)
{

    func(req, res, args...);
}
template <typename FuncType, int N, typename... Args>
typename std::enable_if<sizeof...(Args) != N, void>::type
http_router::callback_call_helper__(const http_server_request& req,
                                    http_server_response& res, FuncType func,
                                    const std::smatch& sm, Args... args)
{

    callback_call_helper__<FuncType, N, Args..., std::string>(
        req, res, func, sm, args..., sm[sizeof...(args)].str());
}
template <typename... StringType>
http_router::http_router(const std::string& pattern,
                         void (*callback)(const http_server_request&,
                                          http_server_response&,
                                          StringType...),
                         const std::vector<std::string>& methods,
                         const int                       priority)
    : http_router(pattern, std::function(callback), methods, priority)
{
}
template <typename... StringType>
http_router::http_router(
    const std::string& pattern,
    std::function<void(const http_server_request&, http_server_response&,
                       StringType...)>
                             callback,
    std::vector<std::string> methods, int priority)
    : priority__(priority)
    , methods__(std::move(methods))
{
    route_callback__ = [=](const http_server_request& req, http_server_response& res,
                           const std::string& url) {
        std::regex  re(pattern);
        std::smatch sm;
        if (!std::regex_match(url, sm, re))
        {

            return false;
        }
        callback_call_helper__<decltype(callback), sizeof...(StringType)>(
            req, res, callback, sm);

        return true;
    };
}

} // namespace skyfire
