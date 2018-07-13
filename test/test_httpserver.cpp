#include "sf_httpserver.h"

using namespace skyfire;

int main(){
    sf_httpserver_config config;
    config.host = "0.0.0.0";
    config.port = 8080;
    config.thread_count = 4;
    sf_http_server server(config);
    server.start();
}