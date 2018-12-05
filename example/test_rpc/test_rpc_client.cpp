
/**
* @version 1.0.0
* @author skyfire
* @mail skyfireitdiy@hotmail.com
* @see http://github.com/skyfireitdiy/sflib
* @file test_rpc_client.cpp

* sflib第一版本发布
* 版本号1.0.0
* 发布日期：2018-10-22
*/

#include <iostream>
#include "sf_rpc_client.hpp"

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
    // 1.创建rpc客户端
    auto client = sf_rpc_client::make_client();
    // 2.连接rpc服务器
    if (!client->connect_to_server("127.0.0.1", 10001))
    {
        std::cout << "connect error" << std::endl;
        return -1;
    }
    // 3.同步调用，无返回值
    client->call<>("print"s);
    std::cout<<"call finished"<<std::endl;
    std::vector<int> data = {9,5,6,7,41,23,4,5,7};
    disp_vec(data);
    // 4.同步调用，返回sf_tri_type<vector<int>>，使用*解引用（需要显式指明返回值类型）
    data = *client->call<std::vector<int>>("add_one"s, data);
    disp_vec(data);
    std::cout<<"---------"<<std::endl;
    // 5.异步调用，第二个参数为参数为rpc函数返回类型的回调函数（需要显式指明返回值类型）
    client->async_call<std::vector<int>>("add_one"s, disp_vec, data);
    getchar();
}
