#include "sf_rpcserver.h"
#include <iostream>

using namespace std;
using namespace skyfire;



int print(int a)
{
    cout<<"print"<<endl;
    return  a;
}


int main()
{
    auto server = std::make_shared<sf_rpcserver<>>();
    server->reg_rpc_func("print", print);
    cout<<server->listen("127.0.0.1",1234)<<endl;
    sf_eventloop eventloop;
    eventloop.exec();
}