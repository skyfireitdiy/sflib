#include "sf_http_base_server.h"
#include "sf_logger.h"
#include <iostream>

using namespace skyfire;
using namespace std;

auto logger = sf_logger::get_instance();

void request_callback(const sf_http_request& req, sf_http_response& res)
{
    auto url = req.get_request_line().url;
    logger->sf_info("request url:", url);
    auto header = req.get_header();
    logger->sf_info("User-Agent", header.get_header_value("User-Agent"));

    sf_http_header res_header;
    res_header.set_header("Content-Type","text/html; charset=utf8");
    res.set_body(to_byte_array(std::string("Hello World")));
}

int main(){
    logger->add_level_stream(SF_LOG_LEVEL::SF_INFO_LEVEL, &cout);
    sf_http_server_config config;
    config.host = "0.0.0.0";
    config.port = 8080;
    config.thread_count = 4;
    sf_http_base_server server(config);

    server.set_request_callback(request_callback);
    server.start();
}