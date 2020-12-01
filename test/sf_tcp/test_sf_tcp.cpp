#include <sf_tcp_client>
#include <sf_test>

sf_test(test_resolve_dns)
{

    auto ret = skyfire::resolve_dns("www.baidu.com");

    test_assert(skyfire::sf_error(ret));

    for (auto ip : std::vector<std::string>(ret))
    {
        sf_info(ip);
    }

    return true;
}

sf_test(test_connect_host)
{
    auto client = skyfire::tcp_client::make_instance();
    auto ret    = client->connect_to_server("www.baidu.com", 80);

    test_assert(ret);
    ret = client->connect_to_server("14.215.177.38", 80);

    test_assert(!ret);

    return true;
}

int main()
{
    return skyfire::run_test();
}