#pragma once
#include <map>
#include <vector>
#include <regex>
#include <mutex>
#include "sf_router.hpp"
#include "sf_http_response.hpp"
#include "sf_http_request.hpp"

namespace skyfire
{
    class sf_http_router : public sf_router
    {
    private:
        std::function<bool(const sf_http_request &,sf_http_response&,const std::string &)> route_callback__;
        const int priority__;
        const std::vector<std::string> methods__;
        std::recursive_mutex methods_mu__;

        template <typename  FuncType, int N, typename ... Args>
        typename std::enable_if<sizeof...(Args) == N, void>::type
        callback_call_helper__(const sf_http_request &req,sf_http_response& res,FuncType func,const std::smatch &sm, Args ... args);

        template <typename  FuncType, int N, typename ... Args>
        typename std::enable_if<sizeof...(Args) != N, void>::type
        callback_call_helper__(const sf_http_request &req,sf_http_response& res,FuncType func,const std::smatch &sm, Args ... args);;
    public:

        template <typename ... StringType>
        sf_http_router(const std::string& pattern,void(*callback)(StringType...),
                       const std::vector<std::string> &methods = {{"*"}} ,int priority = 0);

        template <typename ... StringType>
        sf_http_router(const std::string& pattern,std::function<void(const sf_http_request &,sf_http_response&,StringType...)> callback,
                       const std::vector<std::string> &methods = {{"*"}} ,int priority = 0);


        bool run_route(const sf_http_request &req, sf_http_response &res,const std::string &url, const std::string &method);

        bool operator<(const sf_http_router& router) const;

        int get_priority() const override;
    };
}