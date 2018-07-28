#pragma once

#include "sf_http_base_server.h"
#include "sf_http_router.h"
#include "sf_websocket_router.h"
#include <set>

namespace skyfire
{
    class sf_http_server: public sf_http_base_server,  public enable_shared_from_this<sf_http_server>
    {
    private:
        std::set<std::shared_ptr<sf_http_router>> http_routers__;
        std::set<std::shared_ptr<sf_websocket_router>> websocket_routers__;
        void default_request_callback__(const sf_http_request &req, sf_http_response &res)
        {
            auto req_line = req.get_request_line();
            for(auto &p:http_routers__)
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
                sec_websocket_key+=websocket_sha1_append_str;
                auto sha1_encoded_key = sf_sha1_encode(to_byte_array(sec_websocket_key));
                auto base64_encoded_key = sf_base64_encode(sha1_encoded_key);
                res.get_header().set_header("Upgrade","websocket");
                res.get_header().set_header("Connection","Upgrade");
                res.get_header().set_header("Sec-WebSocket-Accept",sf_string_trim(base64_encoded_key));
                res.get_header().set_header("Server","skyfire websocket server");
                res.get_header().set_header("Date",sf_make_http_time_str());
                //  NOTE 其他头
                //  res.get_header().set_header("Access-Control-Allow-Credentials","true");
                //  res.get_header().set_header("Access-Control-Allow-Headers","Content-Type");
                if(std::find(header_key.begin(),header_key.end(),sf_to_header_key_format("Sec-WebSocket-Protocol")) != header_key.end())
                    res.get_header().set_header("Sec-WebSocket-Protocol",headers.get_header_value("Sec-WebSocket-Protocol"));
                res.set_status(101);
                res.set_http_version(req.get_request_line().http_version);
                res.set_reason("Switching Protocols");
            }
        }

        void default_websocket_binary_data_callback__(SOCKET sock,const std::string& url, const byte_array& data)
        {
            // TODO 默认Websocket二进制数据回调函数
            websocket_param_t param;
            param.url = url;
            param.sock = sock;
            param.binary_data = data;
            param.type = websocket_data_type ::BinaryData;
            param.p_server = shared_from_this();
            for(auto &p:websocket_routers__)
            {
                if(p->run_route(param))
                    return;
            }
        }

        void default_websocket_text_data_callback__(SOCKET sock,const std::string& url, const std::string& data)
        {
            // TODO 默认Websocket文本数据回调函数
            websocket_param_t param;
            param.url = url;
            param.sock = sock;
            param.text_msg = data;
            param.type = websocket_data_type ::TextData;
            param.p_server = shared_from_this();
            for(auto &p:websocket_routers__)
            {
                if(p->run_route(param))
                    return;
            }
        }

        void default_websocket_open_callback__(SOCKET sock,const std::string& url)
        {
            // TODO websocket 打开事件响应
        }

        void default_websocket_close_callback(SOCKET sock,const std::string& url)
        {
            // TODO websocket 关闭事件响应
        }

    private:
        // WARNING 这个函数不应该直接被调用，而是应该使用make_server函数
        explicit sf_http_server(const sf_http_server_config& config):sf_http_base_server(config)
        {
            // NOTE 普通http回调函数
            set_request_callback([=](const sf_http_request& req,sf_http_response& res){
                default_request_callback__(req, res);
            });

            ////////////////////////////////////////////////////////////////////////////////////////////////////////////

            // NOTE websocket 回调函数
            set_websocket_request_callback([=](const sf_http_request& req,sf_http_response& res){
                default_websocket_request_callback__(req,res);
            });
            set_websocket_binary_data_callback([=](SOCKET sock, const std::string& url,const byte_array& data){
                default_websocket_binary_data_callback__(sock, url, data);
            });
            set_websocket_text_data_callback([=](SOCKET sock, const std::string& url,const std::string& data){
                sf_debug("收到消息",data);
                default_websocket_text_data_callback__(sock, url, data);
            });
            set_websocket_open_callback([=](SOCKET sock,const std::string& url){
                default_websocket_open_callback__(sock,url);
            });
            set_websocket_close_callback([=](SOCKET sock,const std::string& url){
                default_websocket_close_callback(sock,url);
            });
        }

    public:

        void add_router(const std::shared_ptr<sf_http_router>& router)
        {
            http_routers__.insert(router);
        }

        void add_router(const std::shared_ptr<sf_websocket_router>& router)
        {
            websocket_routers__.insert(router);
        }

        static std::shared_ptr<sf_http_server> make_server(const sf_http_server_config &config)
        {
            return std::shared_ptr<sf_http_server>(new sf_http_server(config));
        }

        friend sf_websocket_router;
    };
}