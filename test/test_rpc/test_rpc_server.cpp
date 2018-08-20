#include "sf_rpcserver.h"
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
    auto server = std::make_shared<sf_rpcserver<>>();
    server->reg_rpc_func("print", print);
    server->reg_rpc_func("add_one", add_one);
    std::cout<<server->listen("127.0.0.1",10001)<<std::endl;
    sf_eventloop eventloop;
    eventloop.exec();
}