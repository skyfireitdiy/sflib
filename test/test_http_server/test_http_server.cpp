#include "sf_http_base_server.h"
#include "sf_logger.h"
#include <iostream>
#include <sf_http_server.h>

using namespace skyfire;
using namespace std;

auto logger = sf_logger::get_instance();

void root_route(const sf_http_request &req,sf_http_response& res,const std::string& root) {
    sf_http_header header;
    header.set_header("Content-Type", "text/html;charset=utf8");
    res.set_header(header);
    res.set_body(to_byte_array(root  + " hello world"));
}

void path_route(const sf_http_request &req,sf_http_response& res,const std::string&,const std::string&path){
    sf_http_header header;
    header.set_header("Content-Type", "text/html;charset=utf8");
    res.set_header(header);
    res.set_body(to_byte_array(path  + " test"));
}

int main(){
    logger->add_level_stream(SF_LOG_LEVEL::SF_INFO_LEVEL, &cout);
    sf_http_server_config config;
    config.host = "0.0.0.0";
    config.port = 8080;
    config.thread_count = 4;
    sf_http_server server(config);

    server.add_router({
            "/"s,
            root_route,
            std::vector{{"*"s}}
    });

    server.add_router({
            "/(.*)"s,
            path_route,
            std::vector{{"*"s}}
    });

    server.start();
}