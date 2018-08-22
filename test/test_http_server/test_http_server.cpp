#define SF_DEBUG
#include "sf_http_base_server.hpp"
#include "sf_http_static_router.hpp"
#include "sf_logger.hpp"
#include <iostream>
#include <sf_http_server.hpp>

using namespace std::literals;

using namespace skyfire;

void upload_file_route(const sf_http_request &req,sf_http_response& res) {
    sf_debug(req.get_request_line().http_version);
    sf_debug(req.get_request_line().method);
    sf_debug(req.get_request_line().url);
    auto header = req.get_header();
    for(auto &p:header.get_key_list()){
        sf_debug(p,header.get_header_value(p));
    }

    sf_debug(to_string(req.get_body()));

    res.set_body(to_byte_array("upload ok"s));
    res.set_status(200);
}

void websocket_route(const websocket_param_t& param)
{
    if(param.type == websocket_data_type::TextData){
        sf_debug("text:", param.text_msg);
        param.p_server->send_websocket_data(param.sock,"hello:" + param.text_msg);
    } else{
        sf_debug("binary", param.binary_data.size());
    }
}


int main() {
    g_logger->add_level_stream(SF_LOG_LEVEL::SF_INFO_LEVEL, &std::cout);
    sf_http_server_config config;
    config.host = "0.0.0.0";
    config.port = 8080;
    config.thread_count = 8;
    config.request_timeout = 30;
    auto server = sf_http_server::make_server(config);

    server->add_router(std::make_shared<sf_http_router>(
            "/upload_file"s,
            upload_file_route,
            std::vector<std::string>{{"*"s}}
    ));


    server->add_router(make_websocket_router("/ws", websocket_route));

    server->add_router(make_static_router(R"(/home/skyfire/Desktop/test_html)", {{"*"s}}, "utf-8", true));
    server->start();
}