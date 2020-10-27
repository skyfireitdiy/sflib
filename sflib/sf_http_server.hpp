
/**
* @version 1.0.0
* @author skyfire
* @file sf_http_server.hpp

* 
*/



/**
* @author skyfire
* @file sf_http_server.hpp
*/

#pragma once
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-parameter"
#pragma ide diagnostic ignored "bugprone-lambda-function-name"

#include "sf_http_server.h"

namespace skyfire {
inline void http_server::default_request_callback__(
    const http_server_request &req, http_response &res) {
    const auto req_line = req.request_line();
    std::string url;
    http_header_t param;
    std::string frame;
    parse_url(req_line.url, url, param, frame);
    for (auto &p : http_routers__) {
        if (p->run_route(req, res, url, req_line.method)) {
            return;
        }
    }
    res.set_status(404);
}

inline void http_server::add_router(
    const std::shared_ptr<websocket_router> &router) {
    websocket_routers__.insert(router);
}

inline http_server::http_server(const http_server_config &config)
    :    // TODO 有没有更优雅的写法？
      make_instance_t<http_server, http_base_server>(config) {
    // NOTE 普通http回调函数
    set_request_callback(
        [this](const http_server_request &req, http_response &res) {
            debug("http callback");
            default_request_callback__(req, res);
        });

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////

    set_websocket_request_callback(
        [this](const http_server_request &req, http_response &res) {
            default_websocket_request_callback__(req, res);
        });
    set_websocket_binary_data_callback(
        [this](SOCKET sock, const std::string &url, const byte_array &data) {
            default_websocket_binary_data_callback__(sock, url, data);
        });
    set_websocket_text_data_callback(
        [this](SOCKET sock, const std::string &url, const std::string &data) {
            debug("recv", data);
            default_websocket_text_data_callback__(sock, url, data);
        });
    set_websocket_open_callback([this](SOCKET sock, const std::string &url) {
        default_websocket_open_callback__(sock, url);
    });
    set_websocket_close_callback([this](SOCKET sock, const std::string &url) {
        default_websocket_close_callback__(sock, url);
    });
}

// ReSharper disable once CppMemberFunctionMayBeStatic
inline void http_server::default_websocket_close_callback__(
    SOCKET sock, const std::string &url) {
    websocket_param_t param = {};
    parse_url(url, param.url, param.param, param.frame);
    param.sock = sock;
    param.text_msg = "";
    param.type = websocket_data_type ::Close;
    param.p_server = shared_from_this();
    for (auto &p : websocket_routers__) {
        if (p->run_route(param)) return;
    }
    close_websocket(sock);
}

// ReSharper disable once CppMemberFunctionMayBeStatic
inline void http_server::default_websocket_open_callback__(
    SOCKET sock, const std::string &url) {
    websocket_param_t param = {};
    parse_url(url, param.url, param.param, param.frame);
    param.sock = sock;
    param.text_msg = "";
    param.type = websocket_data_type ::Open;
    param.p_server = shared_from_this();
    for (auto &p : websocket_routers__) {
        if (p->run_route(param)) return;
    }
    close_websocket(sock);
}

inline void http_server::default_websocket_text_data_callback__(
    SOCKET sock, const std::string &url, const std::string &data) {
    websocket_param_t param = {};
    parse_url(url, param.url, param.param, param.frame);
    param.sock = sock;
    param.text_msg = data;
    param.type = websocket_data_type ::TextData;
    param.p_server = shared_from_this();
    for (auto &p : websocket_routers__) {
        if (p->run_route(param)) return;
    }
    close_websocket(sock);
}

inline void http_server::default_websocket_binary_data_callback__(
    SOCKET sock, const std::string &url, const byte_array &data) {
    websocket_param_t param = {};
    parse_url(url, param.url, param.param, param.frame);
    param.sock = sock;
    param.binary_data = data;
    param.type = websocket_data_type ::BinaryData;
    param.p_server = shared_from_this();
    for (auto &p : websocket_routers__) {
        if (p->run_route(param)) return;
    }
    close_websocket(sock);
}

inline void http_server::default_websocket_request_callback__(
    const http_server_request &req, http_response &res) const {
    auto headers = req.header();
    auto header_key = headers.key_list();
    // 基于sha加密方式的握手协议
    if (std::find(header_key.begin(), header_key.end(),
                  to_header_key_format("Sec-WebSocket-Key")) !=
        header_key.end()) {
        auto sec_websocket_key = string_trim(headers.header_value("Sec-WebSocket-Key"));
        if (sec_websocket_key.empty()) return;
        sec_websocket_key += websocket_sha1_append_str;
        const auto sha1_encoded_key =
            sha1_encode(to_byte_array(sec_websocket_key));
        const auto base64_encoded_key = base64_encode(sha1_encoded_key);
        res.header().set_header("Upgrade", "websocket");
        res.header().set_header("Connection", "Upgrade");
        res.header().set_header("Sec-WebSocket-Accept",
                                string_trim(base64_encoded_key));
        res.header().set_header("Server", "skyfire websocket server");
        res.header().set_header("Date", make_http_time_str());
        //  NOTE 其他头
        //  res.header().set_header("Access-Control-Allow-Credentials","true");
        //  res.header().set_header("Access-Control-Allow-Headers","Content-Type");
        if (std::find(header_key.begin(), header_key.end(),
                      to_header_key_format("Sec-WebSocket-Protocol")) !=
            header_key.end())
            res.header().set_header(
                "Sec-WebSocket-Protocol",
                headers.header_value("Sec-WebSocket-Protocol"));
        res.set_status(101);
        res.set_http_version(req.request_line().http_version);
    }
}

inline void http_server::add_router(
    const std::shared_ptr<router> &router) {
    http_routers__.insert(router);
}

}    // namespace skyfire
#pragma clang diagnostic pop