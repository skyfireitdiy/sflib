#define SF_DEBUG
#include "sf_http_base_server.h"
#include "sf_http_static_router.h"
#include "sf_logger.h"
#include <iostream>
#include <sf_http_server.h>

using namespace skyfire;
using namespace std;


void root_route(const sf_http_request &req,sf_http_response& res,const std::string& root) {
    sf_debug(root);
    sf_http_header header;
    header.set_header("Content-Type", "text/html;charset=utf-8");
    res.set_header(header);
    res.set_body(to_byte_array(root  + " hello world"));
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
    g_logger->add_level_stream(SF_LOG_LEVEL::SF_INFO_LEVEL, &cout);
    sf_http_server_config config;
    config.host = "0.0.0.0";
    config.port = 8080;
    config.thread_count = 8;
    config.request_timeout = 30;
    auto server = sf_http_server::make_server(config);

    server->add_router(std::make_shared<sf_http_router>(
            "/"s,
            root_route,
            vector<string>{{"*"s}}
    ));

    server->add_router(std::make_shared<sf_http_router>(
            "/hello"s,
            root_route,
            vector<string>{{"*"s}}
    ));


    server->add_router(make_websocket_router("/ws", websocket_route));

    server->add_router(make_static_router("/home/skyfire/code/hhjr-python/public", {{"*"s}}, "utf-8", true));
    server->start();
}