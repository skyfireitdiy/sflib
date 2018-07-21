#pragma once

#include <functional>
#include "sf_http_server_config.h"
#include "sf_eventloop.h"
#include "sf_tcpserver.h"
#include "sf_logger.h"
#include "sf_object.h"
#include "sf_http_request.h"
#include "sf_http_response.h"
#include "sf_http_utils.h"

#include <iostream>

using namespace std;


namespace skyfire {

    class sf_http_base_server {
    private:
        const sf_http_server_config config__;
        std::shared_ptr<sf_tcpserver> server__ = std::make_shared<sf_tcpserver>(true);
        std::function<void(const sf_http_request&,sf_http_response&)> request_callback__;
        std::function<void(const sf_http_request&,sf_http_response&)> websocket_request_callback__;
        std::function<void(SOCKET,const byte_array& data)> websocket_data_callback__;

        std::unordered_map<SOCKET, req_data_t> req_data__;
        std::mutex mu_req_data__;
        std::unordered_map<SOCKET, websocket_data_t> websocket_sock__;

        void raw_data_coming__(SOCKET sock, const byte_array &data)
        {
            if(websocket_sock__.count(sock) != 0)
            {
                if(websocket_data_callback__)
                {
                    websocket_data_callback__(sock,data);
                }
                return;
            }
            {
                std::unique_lock<std::mutex> lck(mu_req_data__);
                cout << "数据到来：\n" << to_string(data) << endl;
                if (req_data__.count(sock) == 0) {
                    req_data__[sock] = req_data_t();
                }
                req_data__[sock].buffer += data;
                if (req_data__[sock].buffer.empty()) {
                    return;
                }
            }
            sf_http_request request(req_data__[sock].buffer);
            if(request.is_valid())
            {
                req_data__[sock].new_req = true;
                req_data__[sock].buffer.clear();
                bool keep_alive = true;

                auto req_header = request.get_header();
                if(sf_equal_nocase_string(req_header.get_header_value("Connection"), "Upgrade"))
                {
                    if(sf_equal_nocase_string(req_header.get_header_value("Upgrade"),"websocket"))
                    {
                        // NOTE 删除记录，防止超时检测线程关闭连接
                        req_data__.erase(sock);
                        websocket_data_t ws_data;
                        ws_data.url = request.get_request_line().url;
                        websocket_sock__.insert({sock,ws_data});
                        if(websocket_request_callback__)
                        {
                            sf_http_response res;
                            websocket_request_callback__(request, res);
                            res.get_header().set_header("Content-Length", std::to_string(res.get_length()));
                            server__->send(sock,res.to_package());
                        }
                    }
                    return;
                }

                if(request_callback__)
                {
                    sf_http_response res;
                    res.set_status(200);
                    res.set_reason("OK");

                    res.set_http_version(request.get_request_line().http_version);
                    request_callback__(request,res);
                    res.get_header().set_header("Content-Length", std::to_string(res.get_length()));
                    if(sf_to_lower_string(req_header.get_header_value("Connection","Close")) ==
                            sf_to_lower_string("Close"))
                    {
                        keep_alive = false;
                    }
                    res.get_header().set_header("Connection",keep_alive?"Keep-Alive":"Close");
                    server__->send(sock,res.to_package());
                    if(!keep_alive)
                    {
                        server__->close(sock);
                        std::unique_lock<std::mutex> lck(mu_req_data__);
                        req_data__.erase(sock);
                    }
                }
            }
        }

        void build_new_request__(SOCKET sock) {
            std::unique_lock<std::mutex> lck(mu_req_data__);
            std::thread([=]() {
                while(true) {
                    std::this_thread::sleep_for(std::chrono::seconds(config__.request_timeout));
                    std::unique_lock<std::mutex> lck(mu_req_data__);
                    if (req_data__.count(sock) != 0) {
                        if (!req_data__[sock].new_req) {
                            server__->close(sock);
                            req_data__.erase(sock);
                        } else {
                            req_data__[sock].new_req = true;
                        }
                    }else{
                        break;
                    }
                }
            }).detach();
        }

        void on_socket_closed(SOCKET sock)
        {
            if(req_data__.count(sock)!=0)
                req_data__.erase(sock);
            if(websocket_sock__.count(sock) != 0)
                websocket_sock__.erase(sock);
        }

    public:
        explicit sf_http_base_server(const sf_http_server_config& config):
                config__ (config)
        {
            sf_bind_signal(server__, raw_data_coming, [=](SOCKET sock, const byte_array &data){
                raw_data_coming__(sock,data);
            }, false);
            sf_bind_signal(server__, new_connection, [=](SOCKET sock){
                build_new_request__(sock);
            }, false);
            sf_bind_signal(server__, closed, [=](SOCKET sock){
                on_socket_closed(sock);
            }, false);

        }

        bool start()
        {
            if(!server__->listen(config__.host,config__.port))
            {
                return false;
            }
            std::vector<std::thread> thread_vec;
            for(auto i=0;i<config__.thread_count;++i)
            {
                thread_vec.emplace_back(thread([=]{
                    sf_eventloop loop;
                    loop.exec();
                }));
            }
            for(auto &p:thread_vec)
            {
                p.join();
            }
            return true;
        }

        void set_request_callback(std::function<void(const sf_http_request&,sf_http_response&)> request_callback)
        {
            request_callback__ = request_callback;
        }

        void set_websocket_request_callback(std::function<void(const sf_http_request&,sf_http_response&)> websocket_request_callback)
        {
            websocket_request_callback__ = websocket_request_callback;
        }

        void set_websocket_data_callback(std::function<void(SOCKET,const byte_array&)> websocket_data_callback)
        {
            websocket_data_callback__ = websocket_data_callback;
        }
    };


}