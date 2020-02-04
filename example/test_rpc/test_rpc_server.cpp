
/**
* @version 1.0.0
* @author skyfire
* @file test_rpc_server.cpp
*/
#define SF_DEBUG
#include "network/sf_rpc_server.hpp"

using namespace skyfire;

void print() { std::cout << "print" << std::endl; }

std::vector<int> add_one(std::vector<int> data)
{
    for (auto& p : data) {
        ++p;
    }
    return data;
}

void output(const char* str) { std::cout << str << std::endl; }

void print_str(byte_array data)
{
    std::cout << skyfire::to_string(data) << std::endl;
}

int main()
{
    // 1.创建server对象
    auto server = sf_rpc_server::make_instance();
    // 2.注册rpc函数
    server->reg_rpc_func("print", print);
    server->reg_rpc_func("add_one", add_one);
    server->reg_rpc_func("print_str", print_str);
    // 3.监听
    std::cout << server->listen("127.0.0.1", 10001) << std::endl;
    sf_eventloop event_loop;
    // 4.启动事件循环
    event_loop.exec();
}