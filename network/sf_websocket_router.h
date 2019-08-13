
/**
* @version 1.0.0
* @author skyfire
* @mail skyfireitdiy@hotmail.com
* @see http://github.com/skyfireitdiy/sflib
* @file sf_websocket_router.h

* sflib第一版本发布
* 版本号1.0.0
* 发布日期：2018-10-22
*/

#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCUnusedGlobalDeclarationInspection"
#pragma once

#include "core/sf_stdc++.h"

#include "core/sf_type.hpp"
#include "network/sf_router.hpp"
#include "sf_websocket_utils.hpp"
#include "tools/sf_utils.h"

namespace skyfire {
/**
 *  @brief Websocket路由
 */
class sf_websocket_router
    : public sf_make_instance_t<sf_websocket_router, sf_router> {
   private:
    const int priority__;
    std::string url__;
    std::function<void(const sf_websocket_param_t &)> callback__;

   public:
    /**
     * 构造函数
     * @param url websocket url
     * @param callback websocket 回调函数，接收websocket_param_t参数
     */
    explicit sf_websocket_router(
        const std::string &url,
        std::function<void(const sf_websocket_param_t &)> callback,
        int priority = 0);

    /**
     * 构造函数
     * @param url websocket url
     * @param callback websocket 回调函数，接收websocket_param_t参数
     */
    explicit sf_websocket_router(const std::string &url,
                                 void (*callback)(const sf_websocket_param_t &),
                                 int priority = 0);

    /**
     * 运行路由
     * @param context websocket参数
     * @return 是否匹配上
     */
    bool run_route(const sf_websocket_param_t &context) const;
};

}    // namespace skyfire
#pragma clang diagnostic pop