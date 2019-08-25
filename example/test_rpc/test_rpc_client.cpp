
/**
* @version 1.0.0
* @author skyfire
* @mail skyfireitdiy@hotmail.com
* @see http://github.com/skyfireitdiy/sflib
* @file test_rpc_client.cpp

* sflib第一版本发布
* 版本号1.0.0
* 发布日期：2018-10-22
*/

#define SF_DEBUG
#include "network/sf_rpc_client.hpp"
using namespace skyfire;
using namespace std::literals;

void disp_vec(std::vector<int> data) {
    for (auto p : data) {
        std::cout << p << " " << std::flush;
    }
    std::cout << std::endl;
}

// 1. 声明接口
RPC_OBJECT(Object){RPC_INTERFACE(print) RPC_INTERFACE(print_str)
                       RPC_INTERFACE(add_one)};

int main() {
    // 2.创建rpc客户端
    auto client = Object::make_instance();
    // 3.连接rpc服务器
    if (!client->connect_to_server("127.0.0.1", 10001)) {
        std::cout << "connect error" << std::endl;
        return -1;
    }
    // 3.同步调用，无返回值
    client->print();
    client->print_str("hello world"s);
    std::cout << "call finished" << std::endl;
    std::vector<int> data = {9, 5, 6, 7, 41, 23, 4, 5, 7};
    disp_vec(data);
    // 4.同步调用
    std::vector<int> data1 = (client->add_one(data));
    disp_vec(data1);
    std::cout << "---------" << std::endl;
    // 5.异步调用，第1个参数为参数为rpc函数返回类型的回调函数（需要显式指明返回值类型）
    client->add_one(std::function(disp_vec), data);
    getchar();
}
