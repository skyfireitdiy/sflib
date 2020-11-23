#include "sf_http_utils"
#include "sf_test"

sf_test(test_parse_client_req_url)
{
    std::string agreement, host, resource;
    short       port;

    skyfire::parse_client_req_url("http://www.baidu.com", agreement, host, port, resource);

    test_str_eq(agreement, "http");
    test_str_eq(host, "www.baidu.com");
    test_str_eq(resource, "/");
    test_num_eq(port, 80);

    return true;
}

int main()
{
    return skyfire::test_run();
}