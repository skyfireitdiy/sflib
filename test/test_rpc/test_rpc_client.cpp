#include <iostream>
#include "sf_rpcclient.h"

using namespace skyfire;
using namespace std::literals;

void disp_vec(std::vector<int> data)
{
    for(auto p: data)
    {
        std::cout<<p<<" "<<std::flush;
    }
    std::cout<<std::endl;
}

int main()
{
    auto client = std::make_shared<sf_rpcclient<>>();
    if (!client->connect("127.0.0.1", 10001))
    {
        std::cout << "connect error" << std::endl;
        return -1;
    }
    client->call<>("print"s);
    std::vector<int> data = {9,5,6,7,41,23,4,5,7};
    disp_vec(data);
    data = *client->call<std::vector<int>>("add_one"s, data);
    disp_vec(data);
    std::cout<<"---------"<<std::endl;
    client->async_call<std::vector<int>>("add_one"s, disp_vec, data);
    system("pause");
}
