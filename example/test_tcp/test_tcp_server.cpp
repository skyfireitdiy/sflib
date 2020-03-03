
/**
* @version 1.0.0
* @author skyfire
* @file test_tcp_server.cpp
*/

//
// Created by skyfire on 2018/8/27 0027.
//
#define SF_DEBUG
#include "network/sf_tcp_server.h"
#include "tools/sf_logger.hpp"

using namespace skyfire;

int main()
{
    // // 1.创建服务器
    // auto server = sf_tcp_server::make_instance(false);
    // // 2.监听
    // if (!server->listen("0.0.0.0", 9988)) {
    //     std::cout << "listen on 9988 error" << std::endl;
    //     return -1;
    // }
    // // 3.设置数据到来事件响应
    // sf_bind_signal(
    //     server, data_coming,
    //     [=](SOCKET sock, const sf_pkg_header_t& header, const byte_array& data) {
    //         sf_debug(header.type, header.length, to_string(data));
    //         server->send(sock, header.type, data);
    //     },
    //     true);
    // // 4. 启动消息循环
    // sf_eventloop loop;
    // loop.exec();
    // 1.创建服务器
    auto server = sf_tcp_server::make_instance(true);
    // 2.监听
    if (!server->listen("0.0.0.0", 9988)) {
        std::cout << "listen on 9988 error" << std::endl;
        return -1;
    }
    // 3.设置数据到来事件响应
    sf_bind_signal(
        server, raw_data_coming,
        [=](SOCKET sock, const byte_array& data) {
            sf_debug(to_string(data));
            std::thread([&server, &sock, &data]() {
                sf_debug("send", to_string(data));
                sf_debug(server->send(sock, data));
            }).join();
        },
        true);
    // 4. 启动消息循环
    sf_eventloop loop;
    loop.exec();
}