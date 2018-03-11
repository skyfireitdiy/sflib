#include "sf_rpcserver.h"
#include <iostream>


using namespace std;
using namespace skyfire;

int add(int a,int b)
{
    return a+b;
}


class rpcserver : public sf_rpcserver<>, public std::enable_shared_from_this<rpcserver>
{
private:
    std::string add_welcome(std::string str)
    {
        return "hello:" + str;
    }
public:
    using this_type = std::enable_shared_from_this<rpcserver>;

#include "rpc_declare.h"
};

int main()
{
    auto server = make_shared<rpcserver>();
    server->listen("127.0.0.1",1234);
    sf_eventloop::get_instance()->exec();
}