
/**
* @version 1.0.0
* @author skyfire
* @mail skyfireitdiy@hotmail.com
* @see http://github.com/skyfireitdiy/sflib
* @file sf_http_server.h

* sflib第一版本发布
* 版本号1.0.0
* 发布日期：2018-10-22
*/

#pragma once

#include "core/sf_stdc++.h"
#include "sf_http_base_server.hpp"
#include "sf_http_part_router.h"
#include "sf_http_router.hpp"
#include "sf_http_static_router.hpp"
#include "sf_router_shared_ptr_compare.hpp"
#include "sf_websocket_router.hpp"
#include "tools/sf_utils.h"

namespace skyfire {
/**
 * @brief  HTTP服务器框架，丰富基础框架的接口
 */

class sf_http_server final
    : public sf_make_instance_t<sf_http_server, sf_http_base_server>,
      public std::enable_shared_from_this<sf_http_server> {
   private:
    std::multiset<std::shared_ptr<sf_router>,
                  sf_router_shared_ptr_compare__<sf_router>>
        http_routers__;

    std::multiset<std::shared_ptr<sf_websocket_router>,
                  sf_router_shared_ptr_compare__<sf_websocket_router>>
        websocket_routers__;

    void default_request_callback__(const sf_http_request &req,
                                    sf_http_response &res);

    void default_websocket_request_callback__(const sf_http_request &req,
                                              sf_http_response &res) const;

    void default_websocket_binary_data_callback__(SOCKET sock,
                                                  const std::string &url,
                                                  const byte_array &data);

    void default_websocket_text_data_callback__(SOCKET sock,
                                                const std::string &url,
                                                const std::string &data);

    void default_websocket_open_callback__(SOCKET sock, const std::string &url);

    void default_websocket_close_callback__(SOCKET sock,
                                            const std::string &url);

   public:
    explicit sf_http_server(
        const sf_http_server_config &config = sf_http_server_config{});

    /**
     * 添加http路由
     * @param router http路由
     */
    void add_router(const std::shared_ptr<sf_router> &router);

    /**
     * 添加websocket路由
     * @param router websocket路由
     */
    void add_router(const std::shared_ptr<sf_websocket_router> &router);

    friend sf_websocket_router;
};
}    // namespace skyfire