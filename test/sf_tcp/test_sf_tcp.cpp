#define SF_DEBUG
#include "sflib/sf_type.h"
#include <sf_tcp_client>
#include <sf_tcp_server>
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

sf_test(test_server_client)
{
    auto server = skyfire::tcp_server::make_instance(skyfire::tcp::raw(true));

    skyfire::eventloop lp;

    sf_bind(server, new_connection, [](SOCKET sock) {
        sf_info("new connection", sock);
    }, false);

    sf_bind(server, raw_data_coming, [&server, &lp](SOCKET sock, skyfire::byte_array data) {
        sf_info("recv:", skyfire::to_string(data));
        server->close(sock);
        server->close();
        lp.quit(); 
    }, false);

    server->listen("127.0.0.1", 9300);

    lp.exec();
    return true;
}

int main()
{
    return skyfire::run_test();
}