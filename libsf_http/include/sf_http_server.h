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
            for(auto &p:routers__)
            {
                if(p->run_route(req,res,req_line.url,req_line.method))
                    return;
            }
        }

        void default_websocket_request_callback__(const sf_http_request &req, sf_http_response &res)
        {
            auto headers = req.get_header();
            auto header_key = headers.get_key_list();
            // 基于sha加密方式的握手协议
            if(std::find(header_key.begin(),header_key.end(),sf_to_header_key_format("Sec-WebSocket-Key")) != header_key.end())
            {
                auto sec_websocket_key = headers.get_header_value("Sec-WebSocket-Key");
                if(sec_websocket_key.empty())
                    return;
                sec_websocket_key.insert(sec_websocket_key.begin(),websocket_sha1_append_str.begin(),websocket_sha1_append_str.end());

                auto sha1_encoded_key = sf_sha1_encode(to_byte_array(sec_websocket_key));

                auto base64_encoded_key = sf_base64_encode(sha1_encoded_key);

                res.get_header().set_header("Upgrade","websocket");
                res.get_header().set_header("Connection","Upgrade");
                res.get_header().set_header("Sec-WebSocket-Accept",sf_string_trim(base64_encoded_key));

                if(std::find(header_key.begin(),header_key.end(),sf_to_header_key_format("Sec-WebSocket-Protocol")) != header_key.end())
                {
                    res.get_header().set_header("Sec-WebSocket-Protocol",headers.get_header_value("Sec-WebSocket-Protocol"));
                }

                res.set_status(101);
                res.set_http_version(req.get_request_line().http_version);
                res.set_reason("Switching Protocols");
            }
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