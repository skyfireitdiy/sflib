#include "sf_rpcserver.h"
#include <iostream>

using namespace std;
using namespace skyfire;

int add(int a,int b)
{
    return a+b;
}

vector<int> sort_vec(vector<int> data)
{
    std::sort(data.begin(),data.end());
    return data;
}

std::string add_welcome(std::string str)
{
    return "hello:" + str;
}

int main()
{
    auto server = std::make_shared<sf_rpcserver<>>();

    server->reg_rpc_func("add"s, add);
    server->reg_rpc_func("add_welcome"s, add_welcome);
    server->reg_rpc_func("sort"s, sort_vec);

    server->listen("127.0.0.1",1234);
    sf_eventloop eventloop;
    eventloop.exec();
}