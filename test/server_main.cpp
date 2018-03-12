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

class rpcserver : public sf_rpcserver<>, public std::enable_shared_from_this<rpcserver>
{
private:
    std::string add_welcome(std::string str)
    {
        return "hello:" + str;
    }
public:
    using this_type = std::enable_shared_from_this<rpcserver>;

    SF_BEGIN_REG_RPC_FUNC
        SF_REG_RPC_FUNC("add",,add,int,int,int);
        SF_REG_RPC_FUNC("add_welcome",,std::bind(add_welcome,
                                                            this_type::shared_from_this(),
                                                            std::placeholders::_1),
                        std::string,std::string);
        SF_REG_RPC_FUNC("sort_vec",, sort_vec, vector<int>, vector<int>);
    SF_END_REG_RPC_FUNC
};

int main()
{
    auto server = make_shared<rpcserver>();
    server->listen("127.0.0.1",1234);
    sf_eventloop::get_instance()->exec();
}