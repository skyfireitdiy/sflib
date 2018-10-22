
/**
* @version 1.0.0
* @author skyfire
* @email skyfireitdiy@hotmail.com
* @see http://github.com/skyfireitdiy/sflib
* @file test_rpc_server.cpp

* sflib第一版本发布
* 版本号1.0.0
* 发布日期：2018-10-22
*/

#include "sf_rpcserver.hpp"
#include <iostream>

using namespace skyfire;

void print()
{
    std::cout<<"print"<<std::endl;
}

std::vector<int> add_one(std::vector<int> data)
{
    for(auto &p: data)
    {
        ++p;
    }
    return data;
}

void output(const char *str)
{
    std::cout<<str<<std::endl;
}

int main()
{
    std::cout<< sizeof(pkg_header_t) << std::endl;
    auto server = sf_rpcserver::make_server();
    server->reg_rpc_func("print", print);
    server->reg_rpc_func("add_one", add_one);
    std::cout<<server->listen("127.0.0.1",10001)<<std::endl;
    sf_eventloop eventloop;
    eventloop.exec();
}