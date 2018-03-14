#include "sf_rpcserver.h"
#include <iostream>

using namespace std;
using namespace skyfire;

int add(int a,int b)
{
    return a+b;
}


int main()
{
    auto server = std::make_shared<sf_rpcserver<>>();
    server->reg_rpc_func("sort"s, [](vector<int> data){
        std::sort(data.begin(),data.end());
        return data;
    });
    cout<<server->listen("127.0.0.1",1234)<<endl;
    sf_eventloop eventloop;
    eventloop.exec();
}