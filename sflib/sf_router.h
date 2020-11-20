
/**
* @version 1.0.0
* @author skyfire
* @file sf_router.h
*/
#pragma once
#pragma clang diagnostic push
#pragma ide diagnostic   ignored "OCUnusedGlobalDeclarationInspection"

#include "sf_http_server_request.h"
#include "sf_http_server_response.h"
#include "sf_stdc++.h"

namespace skyfire
{
/**
 *  @brief 路由
 */
class router
{
public:
    virtual ~router() = default;
    /**
     * 获取优先级
     * @return 优先级
     */
    virtual int priority() const;

    virtual bool run_route(const http_server_request& req, http_server_response& res,
                           const std::string& url, const std::string& method);
};

} // namespace skyfire
#pragma clang diagnostic pop