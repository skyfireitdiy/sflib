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
#include "sf_websocket_utils.h"
#include "sf_logger.h"
#include <utility>
#include <mutex>


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
        std::mutex mu_request_context__;

        std::unordered_map<SOCKET, websocket_context_t> websocket_context__;
        std::recursive_mutex mu_websocket_context__;

        void raw_data_coming__(SOCKET sock, const byte_array &data)
        {
            // 过滤websocket消息
            sf_debug("socket",sock);
            {
                std::lock_guard<std::recursive_mutex> lck(mu_websocket_context__);
                if (websocket_context__.count(sock) != 0) {
                    websocket_data_coming__(sock, data);
                    return;
                }
            }

            {
                std::unique_lock<std::mutex> lck(mu_request_context__);
                sf_debug("Request",to_string(data));
                if (request_context__.count(sock) == 0) {
                    sf_debug("检测为websocket连接");
                    request_context__[sock] = request_context_t();
                }
                request_context__[sock].buffer += data;
                if (request_context__[sock].buffer.empty()) {
                    return;
                }
            }
            // TODO 会不会存在两个请求同时到来导致的粘包现象？
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
                            sf_debug("Response",to_string(res.to_package()));
                            server__->send(sock,res.to_package());
                            std::lock_guard<std::recursive_mutex> lck(mu_websocket_context__);
                            websocket_context__.insert({sock,ws_data});
                            websocket_context__[sock].sock = sock;
                            websocket_context__[sock].buffer = byte_array();
                            if(websocket_open_callback__){
                                websocket_open_callback__(sock, ws_data.url);
                            }
                            sf_debug("websocket add to context", sock);
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
                        std::unique_lock<std::mutex> lck(mu_request_context__);
                        request_context__.erase(sock);
                    }
                }
            } else{
                sf_debug("非法请求或请求不完整");
            }
        }

        template <typename T>
        bool analysis_websocket_pkg(SOCKET sock, const T* header,int &resolve_pos, unsigned long long &len, byte_array& body, bool &fin, int &op_code)
        {
            std::lock_guard<std::recursive_mutex> lck(mu_websocket_context__);
            len = sf_get_size(*header);
            if (websocket_context__[sock].buffer.size() - resolve_pos - sizeof(T) < len) {
                return false;
            }
            sf_debug(len,sizeof(T),websocket_context__[sock].buffer.size());

            body = byte_array(
                    websocket_context__[sock].buffer.begin() + resolve_pos + sizeof(T),
                    websocket_context__[sock].buffer.begin() + resolve_pos + sizeof(T) +
                    len
            );
            if(sf_with_mask(*header)) {
                sf_debug("mask true", *reinterpret_cast<const unsigned int*> (header->mask_key));
                sf_decode_websocket_pkg(body, header->mask_key);
            }
            fin = sf_is_fin(*header);
            op_code = sf_get_op_code(*header);
            resolve_pos += sizeof(T) + len;
            sf_debug("resolve_pos", resolve_pos);
            return true;
        }


        void websocket_data_coming__(int sock, const byte_array &data) {
            // TODO 解析websocket帧，然后发至相应的回调函数
            std::lock_guard<std::recursive_mutex> lck(mu_websocket_context__);
            websocket_context__[sock].buffer += data;
            sf_debug("buffer size", websocket_context__[sock].buffer.size());
            int resolve_pos = 0;

            while (websocket_context__[sock].buffer.size() - resolve_pos >= sizeof(websocket_client_data_1_header_t)) {
                const websocket_client_data_1_header_t *header1 = nullptr;
                const websocket_client_data_2_header_t *header2 = nullptr;
                const websocket_client_data_3_header_t *header3 = nullptr;

                header1 = reinterpret_cast<const websocket_client_data_1_header_t *>(websocket_context__[sock].buffer.data() +
                                                                              resolve_pos);

                bool fin = false;
                int op_code = -1;
                byte_array body;

                // TODO 根据不同的len，获取body
                auto len = sf_get_size(*header1);
                sf_debug("base len", len);
                if (len == 126) {
                    if (websocket_context__[sock].buffer.size() - resolve_pos < sizeof(websocket_client_data_2_header_t)) {
                        break;
                    }
                    header2 = reinterpret_cast<const websocket_client_data_2_header_t *>(
                            websocket_context__[sock].buffer.data() + resolve_pos);
                    if (!analysis_websocket_pkg(sock, header2, resolve_pos, len, body, fin, op_code)) {
                        break;
                    }
                } else if (len == 127) {
                    if (websocket_context__[sock].buffer.size() - resolve_pos < sizeof(websocket_client_data_3_header_t)) {
                        break;
                    }
                    header3 = reinterpret_cast<const websocket_client_data_3_header_t *>(
                            websocket_context__[sock].buffer.data() + resolve_pos);
                    if (!analysis_websocket_pkg(sock, header3, resolve_pos, len, body, fin, op_code)) {
                        break;
                    }
                } else {
                    if (!analysis_websocket_pkg(sock, header1, resolve_pos, len, body, fin, op_code)) {
                        break;
                    }
                }
                websocket_context__[sock].data_buffer += body;
                if (WEBSOCKET_OP_DISCONNECT_PKG == op_code) {
                    server__->close(sock);
                    if(websocket_close_callback__){
                        websocket_close_callback__(sock, websocket_context__[sock].url);
                    }
                    websocket_context__.erase(sock);
                    return;
                }
                if(fin) {
                    if (WEBSOCKET_OP_TEXT_PKG == op_code) {
                        sf_debug("text data", to_string(websocket_context__[sock].data_buffer));
                        if(websocket_text_data_callback__){
                            websocket_text_data_callback__(sock, websocket_context__[sock].url,
                                                           to_string(websocket_context__[sock].data_buffer));
                        }
                    } else if (WEBSOCKET_OP_BINARY_PKG == op_code) {
                        sf_debug("binary data", websocket_context__[sock].data_buffer.size());
                        if(websocket_binary_data_callback__){
                            websocket_binary_data_callback__(sock,websocket_context__[sock].url,
                                                             websocket_context__[sock].data_buffer);
                        }
                    } else if (WEBSOCKET_OP_PING_PKG == op_code) {
                        // TODO ping消息相应
                    } else if (WEBSOCKET_OP_PONG_PKG == op_code) {
                        // TODO pong消息相应
                    }else if (WEBSOCKET_OP_MIDDLE_PKG == op_code) {
                        // TODO middle 消息响应
                    } else {
                        // TODO 其他消息响应
                    }
                    websocket_context__[sock].data_buffer.clear();
                }
            }
            websocket_context__[sock].buffer.erase(websocket_context__[sock].buffer.begin(),websocket_context__[sock].buffer.begin() + resolve_pos);
        }

        void build_new_request__(SOCKET sock) {
            std::unique_lock<std::mutex> lck(mu_request_context__);
            std::thread([=]() {
                while(true) {
                    std::this_thread::sleep_for(std::chrono::seconds(config__.request_timeout));
                    std::unique_lock<std::mutex> lck(mu_request_context__);
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
            std::lock_guard<std::recursive_mutex> lck(mu_websocket_context__);
            if(websocket_context__.count(sock) != 0) {
                if(websocket_close_callback__){
                    websocket_close_callback__(sock, websocket_context__[sock].url);
                }
                websocket_context__.erase(sock);
            }
        }



    protected:
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

    public:

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

        template <typename T>
        bool send_websocket_data(SOCKET sock, const T& data){
            if constexpr (std::is_same_v<T,std::string>) {
                sf_debug("send websocket message", data);
            }
            return server__->send(sock, make_server_websocket_data_pkg(data));
        }

        void close_websocket(SOCKET sock){
            server__->close(sock);
            std::lock_guard<std::recursive_mutex> lck(mu_websocket_context__);
            if(websocket_context__.count(sock) != 0) {
                if(websocket_close_callback__){
                    websocket_close_callback__(sock, websocket_context__[sock].url);
                }
                websocket_context__.erase(sock);
            }
        }

    };


}