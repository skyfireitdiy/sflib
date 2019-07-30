
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

#include "sf_http_base_server.hpp"
#include "sf_http_router.hpp"
#include "sf_websocket_router.hpp"
#include "sf_http_static_router.hpp"
#include "sf_router_shared_ptr_compare.hpp"
#include "core/sf_stdc++.h"
#include "tools/sf_utils.h"


namespace skyfire {
    /**
     * @brief  HTTP服务器框架，丰富基础框架的接口
     */

    class sf_http_server final : public sf_make_instance_t<sf_http_server, sf_http_base_server>,
                                 public std::enable_shared_from_this<sf_http_server> {
    private:
        std::multiset<std::shared_ptr<sf_http_router>, sf_router_shared_ptr_compare__<sf_http_router>> http_routers__;
        std::multiset<std::shared_ptr<sf_websocket_router>, sf_router_shared_ptr_compare__<sf_websocket_router>> websocket_routers__;

        void default_request_callback__(const sf_http_request &req, sf_http_response &res);

        void default_websocket_request_callback__(const sf_http_request &req, sf_http_response &res) const;

        void default_websocket_binary_data_callback__(SOCKET sock, const std::string &url, const byte_array &data);

        void default_websocket_text_data_callback__(SOCKET sock, const std::string &url, const std::string &data);

        void default_websocket_open_callback__(SOCKET sock, const std::string &url) const;

        void default_websocket_close_callback__(SOCKET sock, const std::string &url) const;


    public:
        explicit sf_http_server(const sf_http_server_config &config = sf_http_server_config{});

        /**
         * 添加http路由
         * @param router http路由
         */
        void add_router(const std::shared_ptr<sf_http_router> &router);


        /**
         * 添加websocket路由
         * @param router websocket路由
         */
        void add_router(const std::shared_ptr<sf_websocket_router> &router);


        template<typename ... StringType>
        void add_http_router(const std::string &pattern,
                              std::function<void(const sf_http_request &, sf_http_response &,
                                                 StringType...)> callback,
                              const std::vector<std::string> &methods = {{"*"}},
                              int priority = 0);

        template<typename ...StringType>
        void add_http_router(const std::string &pattern,
                             void(*callback)(const sf_http_request &, sf_http_response &,
                                             StringType...),
                             const std::vector<std::string> &methods = {{"*"}},
                             int priority = 0);

        void
        add_static_router(std::string path, const std::vector<std::string> &methods = { {"*"s} },
                           std::string charset = "utf-8",
                           bool deflate = true,
                           unsigned long long max_file_size = default_http_max_file_size,
                           int priority = default_http_static_priority
        );


        void add_websocket_router(const std::string &url,
                                                                   std::function<void(
                                                                           const sf_websocket_param_t &

                                                                   )> callback,
                                                                   int priority = 0
        );

        void add_websocket_router(const std::string &url, void(*callback)(const sf_websocket_param_t &),

                              int priority = 0
        );


        friend sf_websocket_router;
    };
}