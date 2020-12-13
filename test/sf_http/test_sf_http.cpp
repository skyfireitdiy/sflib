#include "sf_http_utils"
#include "sf_http_client_request"
#include "sf_test"
#include "sf_tcp_client"

sf_test(test_parse_client_req_url)
{
    std::string agreement, host, resource;
    short       port;

    skyfire::parse_client_req_url("http://www.baidu.com", agreement, host, port, resource);

    test_str_eq(agreement, "http");
    test_str_eq(host, "www.baidu.com");
    test_str_eq(resource, "/");
    test_num_eq(port, 80);

    skyfire::parse_client_req_url("http://www.baidu.com:8080/hello/world", agreement, host, port, resource);

    test_str_eq(agreement, "http");
    test_str_eq(host, "www.baidu.com");
    test_num_eq(port, 8080);
    test_str_eq(resource, "/hello/world");

    skyfire::parse_client_req_url("//www.baidu.com:8080/hello/world", agreement, host, port, resource);

    test_str_eq(agreement, "http");
    test_str_eq(host, "www.baidu.com");
    test_num_eq(port, 8080);
    test_str_eq(resource, "/hello/world");

    skyfire::parse_client_req_url("/www.baidu.com:8080/hello/world", agreement, host, port, resource);

    test_str_eq(agreement, "http");
    test_str_eq(host, "/www.baidu.com");
    test_num_eq(port, 8080);
    test_str_eq(resource, "/hello/world");

    skyfire::parse_client_req_url("https://www.baidu.com:8080/hello/world", agreement, host, port, resource);

    test_str_eq(agreement, "https");
    test_str_eq(host, "www.baidu.com");
    test_num_eq(port, 8080);
    test_str_eq(resource, "/hello/world");
}

sf_test(test_build_request)
{
    auto req = skyfire::http_client_request::make_instance();
    req->add_header("Host", "www.baidu.com")->set_method("POST")->set_body(skyfire::to_byte_array("hello world"));

    // TODO 完善用例
}



int main()
{
    return skyfire::run_test();
}