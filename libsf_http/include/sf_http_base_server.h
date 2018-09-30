#pragma once
#include <functional>
#include <vector>
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
        std::recursive_mutex mu_request_context__;
        std::unordered_map<SOCKET, websocket_context_t> websocket_context__;
        std::recursive_mutex mu_websocket_context__;
        std::unordered_map<SOCKET, boundary_data_context_t> boundary_data_context__;
        std::recursive_mutex mu_boundary_data_context__;

        void raw_data_coming__(SOCKET sock, const byte_array &data);

        template <typename T>
        bool analysis_websocket_pkg__(SOCKET sock, const T *header, int &resolve_pos, unsigned long long &len,
                                      byte_array &body, bool &fin, int &op_code);

        void websocket_data_coming__(int sock, const byte_array &data);

        void build_new_request__(SOCKET sock);

        void on_socket_closed__(SOCKET sock);

        byte_array append_boundary_data__(boundary_data_context_t &boundary_data, const byte_array &data);

        void file_response__(SOCKET sock, sf_http_response &res) ;

        void normal_response__(SOCKET sock, sf_http_response &res) ;

        void multipart_response__(SOCKET sock, sf_http_response &res) ;

        static bool check_analysis_multipart_file__(std::vector<sf_http_response::multipart_info_t>& multipart_data);

        void close_request__(SOCKET sock) ;

        void http_handler__(SOCKET sock, sf_http_request http_request);

        void build_boundary_context_data(SOCKET sock, const sf_http_request &request);

        void build_websocket_context_data__(SOCKET sock, const sf_http_request &request) ;

        void send_response_file_part__(SOCKET sock, const sf_http_response::response_file_info_t &file, std::ifstream &fi) ;

    protected:
        explicit sf_http_base_server(sf_http_server_config config);

        void set_request_callback(std::function<void(const sf_http_request&,sf_http_response&)> request_callback);

        void set_websocket_request_callback(std::function<void(const sf_http_request&,sf_http_response&)> websocket_request_callback);

        void set_websocket_binary_data_callback(std::function<void(SOCKET,const std::string &url,const byte_array&)> websocket_binary_data_callback);

        void set_websocket_text_data_callback(std::function<void(SOCKET,const std::string &url,const std::string&)> websocket_text_data_callback);

        void set_websocket_open_callback(std::function<void(SOCKET,const std::string &url)> websocket_open_callback);

        void set_websocket_close_callback(std::function<void(SOCKET,const std::string &url)> websocket_close_callback);

    public:

        bool start();

        template <typename T>
        bool send_websocket_data(SOCKET sock, const T& data);

        template <typename T>
        void send_websocket_data(const T& data);


        void close_websocket(SOCKET sock);
    };
}