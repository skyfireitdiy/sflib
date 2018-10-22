
/**
* @version 1.0.0
* @author skyfire
* @email skyfireitdiy@hotmail.com
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
#include "sf_router_shared_ptr_compare.hpp"
#include <set>


namespace skyfire
{
    /**
     * @brief  HTTP服务器框架，丰富基础框架的接口
     */

    class sf_http_server: public sf_http_base_server,  public std::enable_shared_from_this<sf_http_server>
    {
    private:
        std::set<std::shared_ptr<sf_http_router>,sf_router_shared_ptr_compare__<sf_http_router>> http_routers__;
        std::set<std::shared_ptr<sf_websocket_router>,sf_router_shared_ptr_compare__<sf_websocket_router>> websocket_routers__;
        void default_request_callback__(const sf_http_request &req, sf_http_response &res);

        void default_websocket_request_callback__(const sf_http_request &req, sf_http_response &res);

        void default_websocket_binary_data_callback__(SOCKET sock,const std::string& url, const byte_array& data);

        void default_websocket_text_data_callback__(SOCKET sock,const std::string& url, const std::string& data);

        void default_websocket_open_callback__(SOCKET sock,const std::string& url);

        void default_websocket_close_callback(SOCKET sock,const std::string& url);

    private:
        // WARNING 这个函数不应该直接被调用，而是应该使用make_server函数
        explicit sf_http_server(const sf_http_server_config& config);

    public:
        /**
         * 添加http路由
         * @param router http路由
         */
        void add_router(const std::shared_ptr<sf_http_router>& router);

        /**
         * 添加websocket路由
         * @param router websocket路由
         */
        void add_router(const std::shared_ptr<sf_websocket_router>& router);

        /**
         * 创建http server
         * @param config server配置
         * @return http server对象
         */
        static std::shared_ptr<sf_http_server> make_server(const sf_http_server_config &config);

        friend sf_websocket_router;
    };
}