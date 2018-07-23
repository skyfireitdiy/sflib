#pragma once
#include <map>
#include <vector>
#include <regex>
#include <mutex>
#include "sf_http_response.h"
#include "sf_http_request.h"
#include <iostream>

using namespace std;

namespace skyfire
{
    class sf_http_router
    {
    private:
        std::function<bool(const sf_http_request &,sf_http_response&,const std::string &)> route_callback__;
        const int priority__;
        const std::vector<std::string> methods__;
        std::recursive_mutex methods_mu__;

        template <typename  FuncType, int N, typename ... Args>
        typename std::enable_if<sizeof...(Args) == N, void>::type
        callback_call_helper__(const sf_http_request &req,sf_http_response& res,FuncType func,const std::smatch &sm, Args ... args)
        {
            func(req,res,args...);
        };

        template <typename  FuncType, int N, typename ... Args>
        typename std::enable_if<sizeof...(Args) != N, void>::type
        callback_call_helper__(const sf_http_request &req,sf_http_response& res,FuncType func,const std::smatch &sm, Args ... args)
        {
            callback_call_helper__<FuncType,N,Args...,std::string>(req,res,func,sm,args..., sm[sizeof...(args)].str());
        };
    public:

        template <typename ... StringType>
        sf_http_router(const std::string& pattern,void(*callback)(StringType...),
                       const std::vector<std::string> &methods = {{"*"}} ,int priority = 0):
                sf_http_router(pattern,std::function(callback),methods,priority)
        {

        }

        template <typename ... StringType>
        sf_http_router(const std::string& pattern,std::function<void(const sf_http_request &,sf_http_response&,StringType...)> callback,
                       const std::vector<std::string> &methods = {{"*"}} ,int priority = 0):
            priority__(priority),methods__(methods)
        {
            route_callback__ = [=](const sf_http_request &req,sf_http_response& res,const std::string &url)
            {
                std::regex re(pattern);
                std::smatch sm;
                if(!std::regex_match(url, sm, re))
                {
                    return false;
                }
                callback_call_helper__<decltype(callback),sizeof...(StringType)>(req,res,callback,sm);
                return true;
            };
        }


        bool run_route(const sf_http_request &req, sf_http_response &res,const std::string &url, const std::string &method)
        {
            {
                std::unique_lock<std::recursive_mutex> lck(methods_mu__);
                for (auto &p:methods__)cout << p << &p << endl;
                if (methods__.cend() == std::find(methods__.cbegin(), methods__.cend(), "*"s)) {
                    if (methods__.cend() == std::find(methods__.cbegin(), methods__.cend(), method)) {
                        return false;
                    }
                }
            }
            return route_callback__(req, res, url);
        }

        bool operator<(const sf_http_router& router) const {
            return priority__ < router.priority__;
        }

        int get_priority() const
        {
            return priority__;
        }
    };
}