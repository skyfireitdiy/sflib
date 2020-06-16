
/**
* @version 1.0.0
* @author skyfire
* @file test_msg_bus_server.cpp
*/

#include "sf_msg_bus_server"

using namespace skyfire;

int main() {
    // 1.创建一个消息总线服务器
    auto server = sf_msg_bus_server::make_instance();
    // 2.监听
    server->listen("127.0.0.1", 5678);
    std::string type;
    std::string data;
    while (true) {
        // 3.输入并往总线上投递消息
        std::cout << "type:" << std::flush;
        std::cin >> type;
        if (type == "quit") break;
        std::cout << "data:" << std::flush;
        std::cin >> data;
        server->send_msg(type,
                         to_byte_array(skyfire::to_json(data).to_string()));
    }
    // 4.关闭总线
    server->close();
}