#include <map>
#include <vector>
#include <regex>
#include "sf_http_response.h"
#include "sf_http_request.h"
#include <iostream>

using namespace std;

namespace skyfire
{
    class sf_http_router
    {
    private:
        std::function<bool(const sf_http_request &,sf_http_response&,const std::string &, const std::string &)> route_callback__;
        int priority__;

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
                       const std::vector<std::string> &methods = {{"*"}} ,int priority = 0)
        {
            priority__ = priority;
            cout<<"size:"<<methods.size()<<endl;
            for(auto &p:methods)cout<<"1-->"<<p<<endl;
            route_callback__ = [=](const sf_http_request &req,sf_http_response& res,const std::string &url, const std::string &method)
            {
                cout<<"size:"<<methods.size()<<endl;
                for(const auto &p:methods)cout<<"2-->"<<p<<endl;
                // TODO 为何methods内容会改变？
                if(std::find(methods.cbegin(), methods.cend(), "*"s) == methods.cend())
                {
                    if(std::find(methods.cbegin(), methods.cend(), method) == methods.cend())
                    {
                        return false;
                    }
                }
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


        bool run_route(const sf_http_request &req, sf_http_response &res,const std::string &url, const std::string &method) const
        {
            return route_callback__(req, res, url, method);
        }

        bool operator<(const sf_http_router& router) const {
            return priority__ < router.priority__;
        }
    };
}