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
}

sf_test(test_connect_host)
{
    auto client = skyfire::tcp_client::make_instance();
    auto ret    = client->connect_to_server("www.baidu.com", 80);

    test_assert(ret);
    ret = client->connect_to_server("14.215.177.38", 80);

    test_assert(!ret);
}

std::mutex              mu;
std::condition_variable cv;

sf_test(test_server)
{
    auto server = skyfire::tcp_server::make_instance(skyfire::tcp::raw(true));

    skyfire::eventloop lp;

    sf_bind(
        server, new_connection, [](SOCKET sock) {
            sf_info("new connection", sock);
        },
        false);

    sf_bind(
        server, raw_data_coming, [&server, &lp](SOCKET sock, skyfire::byte_array data) {
            sf_info("recv:", skyfire::to_string(data));
            server->close(sock);
            server->close();
            lp.quit();
        },
        false);

    server->listen("127.0.0.1", 9300);
    cv.notify_one();
    lp.exec();
}

sf_test(test_client)
{
    using namespace std;
    auto client = skyfire::tcp_client::make_instance();

    skyfire::eventloop lp;
    sf_bind(
        client, closed, [&lp]() {
            lp.quit();
        },
        false);
    std::unique_lock<std::mutex> lck(mu);
    cv.wait(lck);
    client->connect_to_server("127.0.0.1", 9300);
    client->send(skyfire::to_byte_array("hello world"s));

    lp.exec();
}

int main()
{
    return skyfire::run_test(4);
}