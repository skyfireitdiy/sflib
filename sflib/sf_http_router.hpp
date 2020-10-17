
/**
* @version 1.0.0
* @author skyfire
* @file sf_http_router.hpp

* 
*/

#pragma clang diagnostic push
#pragma ide diagnostic ignored "bugprone-lambda-function-name"

/**
* @author skyfire
* @file sf_http_router.hpp
*/
#pragma once
#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCUnusedGlobalDeclarationInspection"

#include "sf_http_router.h"

#include <utility>
#include "sf_stdc++.h"
#include "sf_utils.hpp"

namespace skyfire {
template <typename FuncType, int N, typename... Args>
typename std::enable_if<sizeof...(Args) == N, void>::type
http_router::callback_call_helper__(const http_request &req,
                                       http_response &res, FuncType func,
                                       const std::smatch &sm, Args... args) {
    debug("call callback_call_helper__");
    func(req, res, args...);
}

template <typename FuncType, int N, typename... Args>
typename std::enable_if<sizeof...(Args) != N, void>::type
http_router::callback_call_helper__(const http_request &req,
                                       http_response &res, FuncType func,
                                       const std::smatch &sm, Args... args) {
    debug("call callback_call_helper__");
    callback_call_helper__<FuncType, N, Args..., std::string>(
        req, res, func, sm, args..., sm[sizeof...(args)].str());
}

template <typename... StringType>
http_router::http_router(const std::string &pattern,
                               void (*callback)(const http_request &,
                                                http_response &,
                                                StringType...),
                               const std::vector<std::string> &methods,
                               const int priority)
    : http_router(pattern, std::function(callback), methods, priority) {}

template <typename... StringType>
http_router::http_router(
    const std::string &pattern,
    std::function<void(const http_request &, http_response &,
                       StringType...)>
        callback,
    std::vector<std::string> methods, int priority)
    : priority__(priority), methods__(std::move(methods)) {
    route_callback__ = [=](const http_request &req, http_response &res,
                           const std::string &url) {
        debug("call route");
        std::regex re(pattern);
        std::smatch sm;
        if (!std::regex_match(url, sm, re)) {
            debug("match error", pattern, url);
            return false;
        }
        callback_call_helper__<decltype(callback), sizeof...(StringType)>(
            req, res, callback, sm);
        debug("return true");
        return true;
    };
}

inline bool http_router::run_route(const http_request &req,
                                      http_response &res,
                                      const std::string &url,
                                      const std::string &method) {
    {
        debug("compare method");
        std::unique_lock<std::recursive_mutex> lck(methods_mu__);
        using namespace std::literals;
        if (methods__.cend() ==
            std::find(methods__.cbegin(), methods__.cend(), "*"s)) {
            if (methods__.cend() ==
                std::find(methods__.cbegin(), methods__.cend(), method)) {
                debug("method compare error");
                return false;
            }
        }
    }
    return route_callback__(req, res, url);
}

inline int http_router::priority() const { return priority__; }

inline bool http_router::operator<(const http_router &router) const {
    return priority__ < router.priority__;
}

}    // namespace skyfire
#pragma clang diagnostic pop
#pragma clang diagnostic pop