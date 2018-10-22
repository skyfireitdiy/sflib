
/**
* @version 1.0.0
* @author skyfire
* @email skyfireitdiy@hotmail.com
* @see http://github.com/skyfireitdiy/sflib
* @file test_rpc_client.cpp

* sflib第一版本发布
* 版本号1.0.0
* 发布日期：2018-10-22
*/

#include <iostream>
#include "sf_rpcclient.hpp"

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
    auto client = sf_rpcclient::make_client();
    if (!client->connect_to_server("127.0.0.1", 10001))
    {
        std::cout << "connect error" << std::endl;
        return -1;
    }
    client->call<>("print"s);
    std::cout<<"call finished"<<std::endl;
    std::vector<int> data = {9,5,6,7,41,23,4,5,7};
    disp_vec(data);
    data = *client->call<std::vector<int>>("add_one"s, data);
    disp_vec(data);
    std::cout<<"---------"<<std::endl;
    client->async_call<std::vector<int>>("add_one"s, disp_vec, data);
    system("pause");
}
