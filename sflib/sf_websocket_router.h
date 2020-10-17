
/**
* @version 1.0.0
* @author skyfire
* @file sf_websocket_router.h
*/

#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCUnusedGlobalDeclarationInspection"
#pragma once

#include "sf_stdc++.h"

#include "sf_router.hpp"
#include "sf_type.hpp"
#include "sf_websocket_utils.hpp"
#include "sf_utils.h"

namespace skyfire {
/**
 *  @brief Websocket路由
 */
class websocket_router
    : public make_instance_t<websocket_router, router> {
private:
    const int priority__;
    std::string url__;
    std::function<void(const websocket_param_t&)> callback__;

public:
    /**
     * 构造函数
     * @param url websocket url
     * @param callback websocket 回调函数，接收websocket_param_t参数
     * @param priority 优先级
     */
    explicit websocket_router(
        const std::string& url,
        std::function<void(const websocket_param_t&)> callback,
        int priority = 0);

    /**
     * 构造函数
     * @param url websocket url
     * @param callback websocket 回调函数，接收websocket_param_t参数
     * @param priority 优先级
     */
    explicit websocket_router(const std::string& url,
        void (*callback)(const websocket_param_t&),
        int priority = 0);

    /**
     * 运行路由
     * @param context websocket参数
     * @return 是否匹配上
     */
    bool run_route(const websocket_param_t& context) const;
};

} // namespace skyfire
#pragma clang diagnostic pop