#pragma clang diagnostic push
#pragma ide diagnostic ignored "bugprone-lambda-function-name"

/**
* @version 1.0.0
* @author skyfire
* @mail skyfireitdiy@hotmail.com
* @see http://github.com/skyfireitdiy/sflib
* @file sf_http_router.hpp

* sflib第一版本发布
* 版本号1.0.0
* 发布日期：2018-10-22
*/
#pragma once
#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCUnusedGlobalDeclarationInspection"

#include "sf_http_router.h"

#include <utility>
#include "core/sf_stdc++.h"
#include "tools/sf_utils.hpp"

namespace skyfire {
template <typename FuncType, int N, typename... Args>
typename std::enable_if<sizeof...(Args) == N, void>::type
sf_http_router::callback_call_helper__(const sf_http_request &req,
                                       sf_http_response &res, FuncType func,
                                       const std::smatch &sm, Args... args) {
    sf_debug("call callback_call_helper__");
    func(req, res, args...);
}

template <typename FuncType, int N, typename... Args>
typename std::enable_if<sizeof...(Args) != N, void>::type
sf_http_router::callback_call_helper__(const sf_http_request &req,
                                       sf_http_response &res, FuncType func,
                                       const std::smatch &sm, Args... args) {
    sf_debug("call callback_call_helper__");
    callback_call_helper__<FuncType, N, Args..., std::string>(
        req, res, func, sm, args..., sm[sizeof...(args)].str());
}

template <typename... StringType>
sf_http_router::sf_http_router(const std::string &pattern,
                               void (*callback)(const sf_http_request &,
                                                sf_http_response &,
                                                StringType...),
                               const std::vector<std::string> &methods,
                               const int priority)
    : sf_http_router(pattern, std::function(callback), methods, priority) {}

template <typename... StringType>
sf_http_router::sf_http_router(
    const std::string &pattern,
    std::function<void(const sf_http_request &, sf_http_response &,
                       StringType...)>
        callback,
    std::vector<std::string> methods, int priority)
    : priority__(priority), methods__(std::move(methods)) {
    route_callback__ = [=](const sf_http_request &req, sf_http_response &res,
                           const std::string &url) {
        sf_debug("call route");
        std::regex re(pattern);
        std::smatch sm;
        if (!std::regex_match(url, sm, re)) {
            sf_debug("match error", pattern, url);
            return false;
        }
        callback_call_helper__<decltype(callback), sizeof...(StringType)>(
            req, res, callback, sm);
        sf_debug("return true");
        return true;
    };
}

inline bool sf_http_router::run_route(const sf_http_request &req,
                                      sf_http_response &res,
                                      const std::string &url,
                                      const std::string &method) {
    {
        sf_debug("compare method");
        std::unique_lock<std::recursive_mutex> lck(methods_mu__);
        using namespace std::literals;
        if (methods__.cend() ==
            std::find(methods__.cbegin(), methods__.cend(), "*"s)) {
            if (methods__.cend() ==
                std::find(methods__.cbegin(), methods__.cend(), method)) {
                sf_debug("method compare error");
                return false;
            }
        }
    }
    return route_callback__(req, res, url);
}

inline int sf_http_router::priority() const { return priority__; }

inline bool sf_http_router::operator<(const sf_http_router &router) const {
    return priority__ < router.priority__;
}

}    // namespace skyfire
#pragma clang diagnostic pop
#pragma clang diagnostic pop