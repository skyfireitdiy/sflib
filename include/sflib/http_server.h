
#pragma once
#include "http_part_router.h"
#include "http_router.h"
#include "http_server_base.h"
#include "http_static_router.h"
#include "router_shared_ptr_compare.h"
#include "stdc++.h"
#include "utils.h"
#include "websocket_router.h"
namespace skyfire
{
class http_server final
    : public make_instance_t<http_server, http_server_base>,
      public std::enable_shared_from_this<http_server>
{
private:
    std::multiset<std::shared_ptr<router>,
                  router_shared_ptr_compare__<router>>
        http_routers__;
    std::multiset<std::shared_ptr<websocket_router>,
                  router_shared_ptr_compare__<websocket_router>>
         websocket_routers__;
    void default_request_callback__(const http_server_request& req,
                                    http_server_response&      res);
    void default_websocket_request_callback__(const http_server_request& req,
                                              http_server_response&      res) const;
    void default_websocket_binary_data_callback__(SOCKET             sock,
                                                  const std::string& url,
                                                  const byte_array&  data);
    void default_websocket_text_data_callback__(SOCKET             sock,
                                                const std::string& url,
                                                const std::string& data);
    void default_websocket_open_callback__(SOCKET sock, const std::string& url);
    void default_websocket_close_callback__(SOCKET             sock,
                                            const std::string& url);

public:
    explicit http_server(
        const http_server_config& config = http_server_config {});
    void add_router(const std::shared_ptr<router>& router);
    void add_router(const std::shared_ptr<websocket_router>& router);
    friend websocket_router;
};
} // namespace skyfire