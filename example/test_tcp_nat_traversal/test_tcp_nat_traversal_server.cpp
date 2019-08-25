
/**
* @version 1.0.0
* @author skyfire
* @mail skyfireitdiy@hotmail.com
* @see http://github.com/skyfireitdiy/sflib
* @file test_tcp_nat_traversal_server.cpp

* sflib第一版本发布
* 版本号1.0.0
* 发布日期：2018-10-22
*/

#define SF_DEBUG

#include "network/sf_tcp_nat_traversal_server.hpp"

using namespace skyfire;

int main() {
    g_logger->add_level_stream(SF_DEBUG_LEVEL, &std::cout);
    // 1. 创建nat穿透server
    auto pserver = sf_tcp_nat_traversal_server::make_instance();
    std::cout << "port:" << std::flush;
    unsigned short port;
    std::cin >> port;
    // 2.监听端口
    auto ret = pserver->listen("0.0.0.0", port);
    if (ret) {
        std::cout << "监听成功" << std::endl;
    } else {
        std::cout << "监听失败" << std::endl;
    }

    // 3. 启动事件循环
    sf_eventloop loop;
    loop.exec();
}