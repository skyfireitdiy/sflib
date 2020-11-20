
/**
* @version 1.0.0
* @author skyfire
* @file sf_http_part_router.h

* 
*/

#pragma once
#pragma clang diagnostic push
#pragma ide diagnostic   ignored "OCUnusedGlobalDeclarationInspection"

#include "sf_http_router.h"
#include "sf_http_server_request.h"
#include "sf_http_server_response.h"
#include "sf_router.hpp"
#include "sf_router_shared_ptr_compare.h"
#include "sf_stdc++.h"
#include "sf_utils.h"

namespace skyfire
{
/**
 * @brief  http分层路由
 */
class http_part_router final
    : public make_instance_t<http_part_router, router>
{
private:
    const int                                                              priority__ = 10;
    const std::vector<std::string>                                         methods__;
    std::recursive_mutex                                                   methods_mu__;
    std::vector<std::string>                                               prefix__;
    std::function<bool(const http_server_request&, http_server_response&)> callback__;

    std::multiset<std::shared_ptr<router>,
                  router_shared_ptr_compare__<router>>
        endpoint_router__;
    std::multiset<std::shared_ptr<router>,
                  router_shared_ptr_compare__<router>>
        middle_router__;

public:
    /**
     * @brief Construct a new sf http part router object
     *
     * @param prefix 前缀
     * @param callback 回调函数
     * @param methods 请求方法
     * @param priority 优先级
     */
    http_part_router(
        std::string prefix,
        std::function<bool(const http_server_request&, http_server_response&)>
            callback,
        std::vector<std::string> methods  = { { "*" } },
        int                      priority = default_http_part_priority);
    /**
     * @brief Construct a new sf http part router object
     *
     * @param prefix 前缀
     * @param callback 回调函数
     * @param methods 请求方法
     * @param priority 优先级
     */
    http_part_router(const std::string& prefix,
                     bool (*callback)(const http_server_request&,
                                      http_server_response&),
                     const std::vector<std::string>& methods  = { { "*" } },
                     int                             priority = default_http_part_priority);

    /**
     * @brief 添加一个router
     *
     * @param router 路由
     */
    void add_router(std::shared_ptr<router> router);

    /**
     * @brief 添加一个分层路由
     *
     * @param router 路由
     */
    void add_router(std::shared_ptr<http_part_router> router);

    /**
     * @brief 执行路由
     *
     * @param req 请求
     * @param res 响应
     * @param url url
     * @param method 请求方式
     * @return true 通行
     * @return false 拦截
     */
    bool run_route(const http_server_request& req, http_server_response& res,
                   const std::string& url, const std::string& method) override;

    /**
     * @brief 小于比较运算符
     *
     * @param router 另一个路由
     * @return true 此路由比较小（优先级高）
     * @return false 此路由比较大（优先级低）
     */
    bool operator<(const http_part_router& router) const;

    [[nodiscard]] int priority() const override;
};
} // namespace skyfire
#pragma clang diagnostic pop