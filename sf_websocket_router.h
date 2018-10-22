
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

#pragma once

#include <string>
#include <functional>

#include "sf_router.hpp"
#include "sf_websocket_utils.hpp"
#include "sf_type.hpp"


namespace skyfire {
    /**
     *  @brief Websocket路由
     */
    class sf_websocket_router : public sf_router {

    private:
        std::string url__;
        std::function<void(const sf_websocket_param_t &)> callback__;

    public:
        /**
         * 构造函数
         * @param url websocket url
         * @param callback websocket 回调函数，接收websocket_param_t参数
         */
        explicit sf_websocket_router(const std::string &url, std::function<void(const sf_websocket_param_t &)> callback);

        /**
         * 构造函数
         * @param url websocket url
         * @param callback websocket 回调函数，接收websocket_param_t参数
         */
        explicit sf_websocket_router(const std::string &url, void(*callback)(const sf_websocket_param_t &));

        /**
         * 运行路由
         * @param context websocket参数
         * @return 是否匹配上
         */
        bool run_route(const sf_websocket_param_t &context);
    };

}