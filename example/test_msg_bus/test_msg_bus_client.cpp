
/**
* @version 1.0.0
* @author skyfire
* @mail skyfireitdiy@hotmail.com
* @see http://github.com/skyfireitdiy/sflib
* @file test_msg_bus_client.cpp

* sflib第一版本发布
* 版本号1.0.0
* 发布日期：2018-10-22
*/

#include "network/sf_msg_bus_client.hpp"

using namespace skyfire;

int main() {
    // 1. 生成客户端
    auto client = sf_msg_bus_client::make_instance();
    // 2. 连接到消息总线服务器
    client->connect_to_server("127.0.0.1", 5678);
    // 3. 添加事件到来相应
    sf_bind_signal(
        client, msg_come,
        [](std::string, byte_array data) {
            std::string str;
            from_json(sf_json::from_string(to_string(data)), str);
            std::cout << "msg_come:" << str << std::endl;
        },
        true);
    std::string type;
    std::cin >> type;
    // 4. 注册消息
    client->reg_msg_to_bus(type);
    sf_eventloop e;
    // 5.事件循环
    e.exec();
}