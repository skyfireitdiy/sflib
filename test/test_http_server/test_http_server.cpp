#include "sf_http_base_server.h"
#include "sf_http_static_router.h"
#include "sf_logger.h"
#include <iostream>
#include <sf_http_server.h>

using namespace skyfire;
using namespace std;

auto logger = sf_logger::get_instance();

void root_route(const sf_http_request &req,sf_http_response& res,const std::string& root) {
    logger->sf_info(root);
    sf_http_header header;
    header.set_header("Content-Type", "text/html;charset=utf-8");
    res.set_header(header);
    res.set_body(to_byte_array(root  + " hello world"));
}

int main() {
    logger->add_level_stream(SF_LOG_LEVEL::SF_INFO_LEVEL, &cout);
    sf_http_server_config config;
    config.host = "0.0.0.0";
    config.port = 8080;
    config.thread_count = 8;
    config.request_timeout = 30;
    sf_http_server server(config);

    server.add_router(std::make_shared<sf_http_router>(
            "/"s,
            root_route,
            vector<string>{{"*"s}}
    ));

    server.add_router(std::make_shared<sf_http_router>(
            "/hello"s,
            root_route,
            vector<string>{{"*"s}}
    ));


    server.add_router(make_static_router("/home/skyfire/IdeaProjects/hhjr-python/public", {{"*"s}}, "utf-8", false));
    server.start();
}