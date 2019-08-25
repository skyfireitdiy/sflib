
/**
* @version 1.0.0
* @author skyfire
* @mail skyfireitdiy@hotmail.com
* @see http://github.com/skyfireitdiy/sflib
* @file test_tcpserver.cpp

* sflib第一版本发布
* 版本号1.0.0
* 发布日期：2018-10-22
*/

//
// Created by skyfire on 2018/8/27 0027.
//
// #define SF_DEBUG
#include "network/sf_tcp_server.h"

using namespace skyfire;

int main() {
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
            std::cout << "recv:" << to_string(data) << std::endl;
            server->send(sock, data);
        },
        true);
    // 4. 启动消息循环
    sf_eventloop loop;
    loop.exec();
}