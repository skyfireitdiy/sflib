
/**
* @version 1.0.0
* @author skyfire
* @file sf_http_router.h
*/
#pragma once

#include "sf_http_server_request.hpp"
#include "sf_http_server_response.hpp"
#include "sf_router.hpp"
#include "sf_stdc++.h"
#include "sf_utils.h"

namespace skyfire
{
/**
 * @brief  http路由
 */
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
    /**
     * 构造函数
     * @tparam StringType 匹配到的字符串
     * @param pattern 用于匹配url的正则表达式
     * @param callback
     * 回调函数，接收N个string类型参数，分别对应pattern匹配到的字符串
     * @param methods
     * 请求类型集合,"GET"，"POST"，"PUT"，"DELETE"等组合,"*"表示所有请求类型
     * @param priority 路由优先级
     */
    template <typename... StringType>
    http_router(const std::string& pattern,
                void (*callback)(const http_server_request&, http_server_response&,
                                 StringType...),
                const std::vector<std::string>& methods  = { { "*" } },
                int                             priority = default_http_endpoint_priority);

    /**
     * 构造函数
     * @tparam StringType 匹配到的字符串
     * @param pattern 用于匹配url的正则表达式
     * @param callback
     * 回调函数，接收N个string类型参数，分别对应pattern匹配到的字符串
     * @param methods
     * 请求类型集合,"GET"，"POST"，"PUT"，"DELETE"等组合,"*"表示所有请求类型
     * @param priority 路由优先级
     */
    template <typename... StringType>
    http_router(const std::string& pattern,
                std::function<void(const http_server_request&,
                                   http_server_response&, StringType...)>
                                         callback,
                std::vector<std::string> methods  = { { "*" } },
                int                      priority = default_http_endpoint_priority);

    /**
     * 运行路由（由框架调用）
     * @param req 请求
     * @param res 响应
     * @param url url
     * @param method 请求方式
     * @return 是否已处理
     */
    bool run_route(const http_server_request& req, http_server_response& res,
                   const std::string& url, const std::string& method);

    /**
     * 重载小于运算符（主要用于排序）
     * @param router 其他router
     * @return 是否小于
     */
    bool operator<(const http_router& router) const;

    /**
     * 获取优先级
     * @return 优先级
     */
    int priority() const override;
};

} // namespace skyfire
