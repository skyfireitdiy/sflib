#pragma once

#include "sf_http_router.h"

namespace skyfire
{
    template<typename FuncType, int N, typename... Args>
    typename std::enable_if<sizeof...(Args) == N, void>::type
    sf_http_router::callback_call_helper__(const sf_http_request &req, sf_http_response &res, FuncType func,
                                           const std::smatch &sm, Args... args) {
        sf_debug("调用callback_call_helper__");
        func(req,res,args...);
    }

    template<typename... StringType>
    sf_http_router::sf_http_router(const std::string &pattern, void (*callback)(StringType...),
                                   const std::vector<std::string> &methods, int priority):
            sf_http_router(pattern,std::function(callback),methods,priority)
    {

    }

    template<typename... StringType>
    sf_http_router::sf_http_router(const std::string &pattern,
                                   std::function<void(const sf_http_request &, sf_http_response &,
                                                      StringType...)> callback, const std::vector<std::string> &methods,
                                   int priority):
            priority__(priority),methods__(methods)
    {
        route_callback__ = [=](const sf_http_request &req,sf_http_response& res,const std::string &url)
        {
            sf_debug("调用route");
            std::regex re(pattern);
            std::smatch sm;
            if(!std::regex_match(url, sm, re))
            {
                sf_debug("match失败");
                return false;
            }
            callback_call_helper__<decltype(callback),sizeof...(StringType)>(req,res,callback,sm);
            return true;
        };
    }

    inline bool sf_http_router::run_route(const sf_http_request &req, sf_http_response &res, const std::string &url,
                                   const std::string &method) {
        {
            sf_debug("开始对比method");
            std::unique_lock<std::recursive_mutex> lck(methods_mu__);
            using namespace std::literals;
            if (methods__.cend() == std::find(methods__.cbegin(), methods__.cend(), "*"s)) {
                if (methods__.cend() == std::find(methods__.cbegin(), methods__.cend(), method)) {
                    sf_debug("method对比失败");
                    return false;
                }
            }
        }
        return route_callback__(req, res, url);
    }

    inline int sf_http_router::get_priority() const {
        return priority__;
    }

    inline bool sf_http_router::operator<(const sf_http_router &router) const {
        return priority__ < router.priority__;
    }

    template<typename FuncType, int N, typename... Args>
    typename std::enable_if<sizeof...(Args) != N, void>::type
    sf_http_router::callback_call_helper__(const sf_http_request &req, sf_http_response &res, FuncType func,
                                           const std::smatch &sm, Args... args) {
        sf_debug("调用callback_call_helper__");
        callback_call_helper__<FuncType,N,Args...,std::string>(req,res,func,sm,args..., sm[sizeof...(args)].str());
    }
}