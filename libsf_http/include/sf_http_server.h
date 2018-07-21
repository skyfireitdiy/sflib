#pragma once

#include "sf_http_base_server.h"
#include "sf_http_router.h"
#include <set>

#include <iostream>

using namespace std;

namespace skyfire
{
    class sf_http_server: public sf_http_base_server
    {
    private:
        std::set<std::shared_ptr<sf_http_router>> routers__;
        void default_dequest_callback__(const sf_http_request &req, sf_http_response &res)
        {
            auto req_line = req.get_request_line();
            cout<<req_line.url<<endl;
            for(auto &p:routers__)
            {
                if(p->run_route(req,res,req_line.url,req_line.method))
                    return;
            }
        }

        void default_websocket_request_callback__(const sf_http_request &req, sf_http_response &res)
        {
            // TODO 默认websocket请求回调函数
        }

        void default_websocket_data_callback__(SOCKET sock, const byte_array& data)
        {
            // TODO 默认Websocket数据回调函数
        }
    public:
        explicit sf_http_server(const sf_http_server_config& config):sf_http_base_server(config)
        {
            set_request_callback([=](const sf_http_request& req,sf_http_response& res){
                default_dequest_callback__(req, res);
            });
            set_websocket_request_callback([=](const sf_http_request& req,sf_http_response& res){
                default_websocket_request_callback__(req,res);
            });
            set_websocket_data_callback([=](SOCKET sock, const byte_array& data){
                default_websocket_data_callback__(sock, data);
            });
        }

        void add_router(const std::shared_ptr<sf_http_router>& router)
        {
            routers__.insert(router);
        }
    };
}