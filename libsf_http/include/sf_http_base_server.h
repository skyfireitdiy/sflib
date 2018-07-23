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

#include <utility>



namespace skyfire {

    class sf_http_base_server {
    private:
        const sf_http_server_config config__;
        std::shared_ptr<sf_tcpserver> server__ = std::make_shared<sf_tcpserver>(true);
        std::function<void(const sf_http_request&,sf_http_response&)> request_callback__;

        std::function<void(const sf_http_request&,sf_http_response&)> websocket_request_callback__;

        std::function<void(SOCKET,const std::string &url,const byte_array& data)> websocket_binary_data_callback__;
        std::function<void(SOCKET,const std::string &url,const std::string& data)> websocket_text_data_callback__;
        std::function<void(SOCKET,const std::string &url)> websocket_open_callback__;
        std::function<void(SOCKET,const std::string &url)> websocket_close_callback__;


        std::unordered_map<SOCKET, request_context_t> request_context__;
        std::mutex mu_req_data__;

        // TODO 以后可能需要加锁
        std::unordered_map<SOCKET, websocket_context_t> websocket_context__;

        void raw_data_coming__(SOCKET sock, const byte_array &data)
        {
            if(websocket_context__.count(sock) != 0)
            {
                // TODO 解析websocket帧，然后发至相应的回调函数
                return;
            }
            {
                std::unique_lock<std::mutex> lck(mu_req_data__);
                cout << "请求：\n" << to_string(data) << endl;
                if (request_context__.count(sock) == 0) {
                    request_context__[sock] = request_context_t();
                }
                request_context__[sock].buffer += data;
                if (request_context__[sock].buffer.empty()) {
                    return;
                }
            }
            sf_http_request request(request_context__[sock].buffer);
            if(request.is_valid())
            {
                request_context__[sock].new_req = true;
                request_context__[sock].buffer.clear();
                bool keep_alive = true;

                auto req_header = request.get_header();

                auto connection_header = req_header.get_header_value("Connection");

                auto connection_header_list = sf_split_string(connection_header, ",");

                // 过滤Connection为Upgrade的请求
                if(std::find_if(connection_header_list.begin(),connection_header_list.end(),[](const std::string& str){
                    return sf_equal_nocase_string(sf_string_trim(str),"Upgrade");
                })!=connection_header_list.end())
                {
                    // 筛选Websocket请求
                    if(sf_equal_nocase_string(req_header.get_header_value("Upgrade"),"websocket"))
                    {
                        // NOTE 删除记录，防止超时检测线程关闭连接
                        request_context__.erase(sock);
                        websocket_context_t ws_data;
                        ws_data.url = request.get_request_line().url;

                        if(websocket_request_callback__)
                        {
                            sf_http_response res;
                            websocket_request_callback__(request, res);
                            res.get_header().set_header("Content-Length", std::to_string(res.get_length()));
                            cout<<"回应:\n"<<to_string(res.to_package())<<endl;
                            server__->send(sock,res.to_package());
                            websocket_context__.insert({sock,ws_data});
                            websocket_context__[sock].sock = sock;
                            websocket_context__[sock].buffer = byte_array();
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
                    if(sf_equal_nocase_string(req_header.get_header_value("Connection","Close"), "Close"))
                    {
                        keep_alive = false;
                    }
                    res.get_header().set_header("Connection",keep_alive?"Keep-Alive":"Close");
                    cout<<"回应:\n"<<to_string(res.to_package())<<endl;
                    server__->send(sock,res.to_package());
                    if(!keep_alive)
                    {
                        server__->close(sock);
                        std::unique_lock<std::mutex> lck(mu_req_data__);
                        request_context__.erase(sock);
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
                    if (request_context__.count(sock) != 0) {
                        if (!request_context__[sock].new_req) {
                            server__->close(sock);
                            request_context__.erase(sock);
                        } else {
                            request_context__[sock].new_req = true;
                        }
                    }else{
                        break;
                    }
                }
            }).detach();
        }

        void on_socket_closed(SOCKET sock)
        {
            if(request_context__.count(sock)!=0)
                request_context__.erase(sock);
            if(websocket_context__.count(sock) != 0)
                websocket_context__.erase(sock);
        }

    public:
        explicit sf_http_base_server(sf_http_server_config config):
                config__ (std::move(config))
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
            request_callback__ = std::move(request_callback);
        }

        void set_websocket_request_callback(std::function<void(const sf_http_request&,sf_http_response&)> websocket_request_callback)
        {
            websocket_request_callback__ = std::move(websocket_request_callback);
        }

        void set_websocket_binary_data_callback(std::function<void(SOCKET,const std::string &url,const byte_array&)> websocket_binary_data_callback)
        {
            websocket_binary_data_callback__ = std::move(websocket_binary_data_callback);
        }

        void set_websocket_text_data_callback(std::function<void(SOCKET,const std::string &url,const std::string&)> websocket_text_data_callback)
        {
            websocket_text_data_callback__ = std::move(websocket_text_data_callback);
        }

        void set_websocket_open_callback(std::function<void(SOCKET,const std::string &url)> websocket_open_callback)
        {
            websocket_open_callback__ = std::move(websocket_open_callback);
        }
        void set_websocket_close_callback(std::function<void(SOCKET,const std::string &url)> websocket_close_callback)
        {
            websocket_close_callback__ = std::move(websocket_close_callback);
        }


    };


}