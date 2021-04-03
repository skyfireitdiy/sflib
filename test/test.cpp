
#if 1
#include <sflib>
using namespace skyfire;
using namespace std;

int main()
{
    run_test(16);
}

sf_test(dns, test_parse_client_req_url)
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

sf_test(http_client, test_build_request_header_to_string)
{
    auto req = skyfire::http_client_request::make_instance();
    req->add_header("Host", "www.baidu.com")->set_url("http://www.baidu.com")->set_method("POST")->set_body(skyfire::to_byte_array("hello world"));
    // TODO 完善用例

    test_np_eq(req->to_byte_array(), to_byte_array("POST / HTTP/1.1\r\nHost:www.baidu.com\r\n\r\nhello world"));
}

sf_test(chan, test_chan_order)
{
    auto             ch = chan<int>::make_instance();
    std::vector<int> data;
    std::vector<int> result { 1, 2, 3, 4, 5 };
    auto             th1 = std::thread([&ch] {
        for (int i = 0; i < 5; ++i)
        {
            (i + 1) >> ch;
        }
    });
    auto             th2 = std::thread([&data, &ch] {
        for (int i = 0; i < 5; ++i)
        {
            int t = 100;
            ch >> t;
            data.push_back(t);
        }
    });
    th1.join();
    th2.join();
    test_np_eq(data, result);
}

sf_test(chan, test_chan_order_with_buffer)
{
    auto             ch = chan<int>::make_instance(5);
    std::vector<int> data;
    std::vector<int> result { 1, 2, 3, 4, 5 };
    auto             th1 = std::thread([&ch] {
        for (int i = 0; i < 5; ++i)
        {
            (i + 1) >> ch;
        }
    });
    auto             th2 = std::thread([&data, &ch] {
        for (int i = 0; i < 5; ++i)
        {
            int t = 100;
            ch >> t;
            data.push_back(t);
        }
    });
    th1.join();
    th2.join();
    test_np_eq(data, result);
}

sf_test(chan, test_write_to_closed_chan)
{
    auto   ch  = chan<int>::make_instance(5);
    auto   ret = true;
    thread th1([ch]() {
        ch->close();
    });
    thread th2([ch, &ret]() {
        this_thread::sleep_for(chrono::seconds(3));
        ret = 5 >> ch;
    });
    th1.join();
    th2.join();
    test_assert(!ret);
}

sf_test(chan, test_read_from_closed_chan)
{
    auto   ch  = chan<int>::make_instance(5);
    auto   ret = true;
    thread th1([ch, &ret]() {
        this_thread::sleep_for(chrono::seconds(3));
        int t;
        ret = ch >> t;
    });
    thread th2([ch]() {
        5 >> ch;
        ch->close();
    });
    th1.join();
    th2.join();
    test_assert(!ret);
}

#ifdef TEST_DNS
sf_test(dns, test_resolve_dns)
{
    auto ret = skyfire::resolve_dns("www.baidu.com");
    test_assert(skyfire::sf_error(ret));
    bool ok = false;
    for (auto ip : std::vector<std::string>(ret))
    {
        ok = true;
    }
    test_assert(ok);
}

sf_test(dns, test_connect_host)
{
    auto client = skyfire::tcp_client::make_instance();
    auto ret    = client->connect_to_server("www.baidu.com", 80);
    test_assert(ret);
    ret = client->connect_to_server("14.215.177.38", 80);
    test_assert(!ret);
}
#endif

std::mutex              mu;
std::condition_variable cv;
sf_test(tcp, test_server)
{
    auto               server = skyfire::tcp_server::make_instance(skyfire::tcp::raw(true));
    skyfire::eventloop lp;
    bool               connected = false;
    byte_array         data;
    sf_bind(
        server, new_connection, [&connected](SOCKET sock) {
            connected = true;
        },
        false);
    sf_bind(
        server, raw_data_coming, [&server, &lp, &data](SOCKET sock, skyfire::byte_array d) {
            data = d;
            server->close(sock);
            server->close();
            lp.quit();
        },
        false);
    auto listened = server->listen("127.0.0.1", 9300);
    cv.notify_one();
    lp.exec();
    test_assert(connected && listened);
    test_p_eq(skyfire::to_string(data), "hello world"s);
}

sf_test(tcp, test_client)
{
    using namespace std;
    auto               client = skyfire::tcp_client::make_instance();
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

sf_test(finally, test_finally_order)
{
    int a = 10;
    {
        sf_finally([&]() {
            a *= 10;
        });
        {
            sf_finally([&]() {
                a += 10;
            });
        }
    }
    test_p_eq(a, 200);
}

sf_test(finally, test_finally_order2)
{
    int a = 10;
    {
        sf_finally([&]() {
            a *= 10;
        });
        sf_finally([&]() {
            a += 10;
        });
    }
    test_p_eq(a, 200);
}

sf_test(buffer, test_read_write)
{
    skyfire::data_buffer buffer;
    auto                 data = skyfire::to_byte_array("hello world");
    buffer.write(data);
    auto t = buffer.read(1024);
    test_np_eq(data, std::get<1>(t));
}

sf_test(buffer, test_read_empty_buffer)
{
    skyfire::data_buffer buffer;
    auto                 t = buffer.read(1024);
    test_np_eq((skyfire::sf_error { { skyfire::err_finished, "" } }), std::get<0>(t));
}

sf_test(buffer, test_read_twice)
{
    skyfire::data_buffer buffer;
    buffer.set_data(skyfire::to_byte_array("hello world"));
    auto t = buffer.read(5);
    test_assert(std::get<0>(t));
    test_num_eq(std::get<1>(t).size(), 5U);
    t = buffer.read(10);
    test_num_eq(skyfire::to_string(t).length(), 6U);
    test_str_eq(skyfire::to_string(t), " world");
}

sf_test(buffer, test_pipe)
{
    auto reader = skyfire::data_buffer::make_instance();
    auto writer = skyfire::data_buffer::make_instance();
    auto pipe   = skyfire::data_buffer::make_instance();
    pipe->set_reader(writer);
    reader->set_reader(pipe);
    writer->write(skyfire::to_byte_array("hello world"));
    auto t = reader->read(1024);
    using namespace std::string_literals;
    test_str_eq(std::get<1>(t).data(), "hello world"s);
}
struct test_argv_param
{
    std::vector<std::string> args;
    json                     result;
};

sf_test(argv, make_parser)
{
    test_assert(argparser::make_parser("") != nullptr);
}

sf_test(argv, check_none_position_json_name_duplicate)
{
    auto parser = argparser::make_parser("");
    parser->add_argument("--append", { argv::json_name("append") });
    test_assert(!parser->add_argument("--append", { argv::json_name("append") }));
}

sf_test(argv, check_position_json_name_duplicate)
{
    auto parser = argparser::make_parser("");
    parser->add_argument("append", { argv::json_name("append") });
    test_assert(!parser->add_argument("--append", { argv::json_name("append") }));
}

sf_test(argv, check_none_position_arg_short_or_long_name_duplicate_short)
{
    auto parser = argparser::make_parser("");
    parser->add_argument("--append", { argv::short_name("-a") });
    test_assert(!parser->add_argument("--batch", { argv::short_name("-a") }));
}

sf_test(argv, check_none_position_arg_short_or_long_name_duplicate_long)
{
    auto parser = argparser::make_parser("");
    parser->add_argument("--append", { argv::short_name("-b") });
    test_assert(!parser->add_argument("--append", { argv::short_name("-a") }));
}

sf_test(argv, gen_json_name)
{
    auto parser = argparser::make_parser("");
    test_assert(!parser->add_argument("", ""));
}

sf_test_p(argv, test_parser_none_postion, test_argv_param, {
                                                               {
                                                                   { "-t", "hello" },
                                                                   R"({"--test":"hello"})"_json,
                                                               },
                                                               {
                                                                   { "--test", "hello" },
                                                                   R"({"--test":"hello"})"_json,
                                                               },
                                                           })
{
    auto parser = argparser::make_parser();
    parser->add_argument("--test", { argv::short_name("-t") });
    auto result = parser->parse_argv(test_param.args, false);
    test_p_eq(json(result), test_param.result);
}

sf_test_p(argv, test_other_name, test_argv_param, {
                                                      {
                                                          { "-t", "hello" },
                                                          R"({"other":"hello"})"_json,
                                                      },
                                                      {
                                                          { "--test", "hello" },
                                                          R"({"other":"hello"})"_json,
                                                      },
                                                  })
{
    auto parser = argparser::make_parser();
    parser->add_argument("--test", { argv::short_name("-t"), argv::required(), argv::json_name("other") });
    auto result = parser->parse_argv(test_param.args, false);
    test_p_eq(json(result), test_param.result);
}

sf_test_p(argv, test_param_type, test_argv_param, {
                                                      {
                                                          { "-s", "string value", "-a", "elem1", "-a", "elem2", "-a", "elem3", "-b", "true", "-n", "26.4" },
                                                          R"({"--string":"string value", "--array":["elem1","elem2","elem3"], "--boolean":true, "--number":26.4})"_json,
                                                      },
                                                  })
{
    auto parser = argparser::make_parser();
    parser->add_argument("--string", { argv::short_name("-s"), argv::type(json_type::string) });
    parser->add_argument("--number", { argv::short_name("-n"), argv::type(json_type::number) });
    parser->add_argument("--array", { argv::short_name("-a"), argv::type(json_type::array) });
    parser->add_argument("--boolean", { argv::short_name("-b"), argv::type(json_type::boolean) });
    auto result = parser->parse_argv(test_param.args, false);
    test_p_eq(json(result), test_param.result);
}

sf_test_p(argv, test_bool_value, test_argv_param, {
                                                      {
                                                          { "-b", "hello" },
                                                          R"({"--boolean":true})"_json,
                                                      },
                                                      {
                                                          { "-b", "0" },
                                                          R"({"--boolean":false})"_json,
                                                      },
                                                      {
                                                          { "-b", "false" },
                                                          R"({"--boolean":false})"_json,
                                                      },
                                                      {
                                                          { "-b", "true" },
                                                          R"({"--boolean":true})"_json,
                                                      },
                                                      {
                                                          { "-b", "1" },
                                                          R"({"--boolean":true})"_json,
                                                      },
                                                  })
{
    auto parser = argparser::make_parser();
    parser->add_argument("-b", "--boolean", json_type::boolean);
    auto result = parser->parse_argv(test_param.args, false);
    test_p_eq(json(result), test_param.result);
}

sf_test_p(argv, test_array, test_argv_param, {
                                                 {
                                                     {},
                                                     R"({"--array":[]})"_json,
                                                 },
                                                 {
                                                     { "-a", "data1" },
                                                     R"({"--array":["data1"]})"_json,
                                                 },
                                                 {
                                                     { "-a", "data1", "-a", "data2", "-a", "data3" },
                                                     R"({"--array":["data1", "data2", "data3"]})"_json,
                                                 },
                                             })
{
    auto parser = argparser::make_parser();
    parser->add_argument("-a", "--array", json_type::array);
    auto result = parser->parse_argv(test_param.args, false);
    test_p_eq(json(result), test_param.result);
}

sf_test_p(argv, test_not_required, test_argv_param, {
                                                        {
                                                            {},
                                                            "{}"_json,
                                                        },
                                                        {
                                                            { "-s", "string value" },
                                                            R"({"--string": "string value"})"_json,
                                                        },
                                                    })
{
    auto parser = argparser::make_parser();
    parser->add_argument("-s", "--string", json_type::string, false);
    auto result = parser->parse_argv(test_param.args, false);
    test_p_eq(json(result), test_param.result);
}

sf_test_p(argv, test_save_bool, test_argv_param, {
                                                     {
                                                         {},
                                                         R"({"--true":false, "--false":true})"_json,
                                                     },
                                                     {
                                                         { "-t" },
                                                         R"({"--true":true, "--false":true})"_json,
                                                     },
                                                     {
                                                         { "-f" },
                                                         R"({"--true":false, "--false":false})"_json,
                                                     },
                                                     {
                                                         { "-t", "-f" },
                                                         R"({"--true":true, "--false":false})"_json,
                                                     },
                                                 })
{
    auto parser = argparser::make_parser();
    parser->add_argument("-t", "--true", json_type::string, true, {}, "", argv_action::store_true);
    parser->add_argument("-f", "--false", json_type::string, true, {}, "", argv_action::store_false);
    auto result = parser->parse_argv(test_param.args, false);
    test_p_eq(json(result), test_param.result);
}

sf_test_p(argv, test_default_value, test_argv_param, {
                                                         {
                                                             {},
                                                             R"({"--string":"default value"})"_json,
                                                         },
                                                         {
                                                             { "-s", "my value" },
                                                             R"({"--string":"my value"})"_json,
                                                         },
                                                     })
{
    auto parser = argparser::make_parser();
    parser->add_argument("-s", "--string", json_type::string, true, json("default value"));
    auto result = parser->parse_argv(test_param.args, false);
    test_p_eq(json(result), test_param.result);
}

sf_test_p(argv, test_subparser, test_argv_param, {
                                                     {
                                                         { "sub1", "-s", "string", "-o", "one" },
                                                         R"({"sub1":{"--string":"string", "--one": "one"}})"_json,
                                                     },
                                                     {
                                                         { "sub2", "-t", "two", "-s", "string" },
                                                         R"({"sub2":{"--string":"string", "--two": "two"}})"_json,
                                                     },
                                                 })
{
    auto parser = argparser::make_parser();
    parser->add_argument("-s", "--string", json_type::string, true, json("default value"));
    auto sub_parser1 = argparser::make_parser();
    parser->add_sub_parser("sub1", sub_parser1);
    auto sub_parser2 = argparser::make_parser();
    parser->add_sub_parser("sub2", sub_parser2);
    sub_parser1->add_argument("-o", "--one");
    sub_parser2->add_argument("-t", "--two");
    auto result = parser->parse_argv(test_param.args, false);
    test_p_eq(json(result), test_param.result);
}

sf_test(sf_string, test_sf_string_repeat)
{
    test_p_eq(sf_string::repeat("1", 6), sf_string("111111"));
}

sf_test(sf_string, test_sf_string_operator)
{
    test_p_eq(sf_string("h") * 5, "hhhhh");
}

sf_test(sf_string, test_sf_string_output)
{
    test_assert((std::cout << sf_string::repeat("hello", 5) << std::endl).good());
}
class test_object : public skyfire::object
{
    sf_singal(s1);
    sf_singal(s2, int, float, double);
};
sf_test(waiter, none_param_event_waiter_test)
{
    test_object t;
    std::thread([&t]() {
        std::this_thread::sleep_for(std::chrono::seconds(5));
        t.s1();
    }).detach();
    sf_wait(&t, s1);
}

sf_test(waiter, many_param_event_waiter_test)
{
    test_object t;
    std::thread([&t]() {
        std::this_thread::sleep_for(std::chrono::seconds(5));
        t.s2(5, 6.5, 504.2);
    }).detach();
    sf_wait(&t, s2);
}

sf_test(multi_value, test_multi_value)
{
    const int         a = 5;
    const std::string b = "hello world";
    double            c = 5.6;
    float             d = 9.5f;
    auto              e = multi_value(a, b, c, d);
    test_p_eq(a, (int)e);
    test_p_eq(b, (std::string)e);
    test_p_eq(c, (double)e);
    test_p_eq(d, (float)e);
}

sf_test(json, test_construct)
{
    json js = 5;
    test_num_eq(static_cast<int>(js), 5);
    js = 6.5;
    test_num_eq(static_cast<double>(js), 6.5);
    js = "hello world"s;
    test_str_eq(static_cast<std::string>(js), "hello world");
    js = true;
    test_assert(js);
    js = false;
    test_assert(!js);
}

sf_test(json, test_insert)
{
    json js;
    js["name"]    = "zhangsan";
    js["age"]     = 30;
    js["man"]     = false;
    js["address"] = json {};
    test_assert(js["address"].is_null());
}

sf_test(json, test_compare)
{
    json js1;
    js1["name"] = "zhangsan";
    js1["age"]  = 30;
    json js2;
    js2["name"] = "zhangsan";
    js2["age"]  = 30;
    test_p_eq(js1, js2);
    // at 不会新增元素
    test_p_eq(js1.at("unknown"), json {});
    test_p_eq(js1, js2);
    // []会新增元素
    test_p_eq(js1["unknown"], json {});
    test_p_neq(js1, js2);
}

sf_test(json, test_parse)
{
    auto js = R"(
    {
        "name":"王小二",
        "age":25,
        "birthday":"1990-01-01",
        "school":"蓝翔",
        "major":["理发","挖掘机"],
        "has_girlfriend":false,
        "car":null,
        "house":null
    }
    )"_json;
    test_str_eq(std::string(js.at("name")), "王小二"s);
    test_num_eq(int(js.at("age")), 25);
    test_str_eq(std::string(js.at("birthday")), "1990-01-01");
    test_str_eq(std::string(js.at("school")), "蓝翔");
    test_str_eq(std::string(js.at("major").at(size_t(0))), "理发");
    test_str_eq(std::string(js.at("major").at(size_t(1))), "挖掘机");
    test_assert(!js.at("has_girlfriend"));
    test_assert(js.at("car").is_null());
    test_assert(js.at("house").is_null());
}

sf_test(json, test_convert)
{
    json js;
    js.convert_to_object();
    test_np_eq(js.type(), json_type::object);
    js.convert_to_array();
    test_np_eq(js.type(), json_type::array);
    js.convert_to_null();
    test_assert(js.is_null());
}
struct body_data
{
    int tall;
    int weight;
};
sf_test(cache, normal_test)
{
    cache cache;
    using namespace std;
    cache.set_data("name", "zhangsan"s);
    cache.set_data("age", 25);
    cache.set_data("body", body_data { 175, 60 });
    auto body = cache.data<body_data>("body");
    test_p_eq(*cache.data<std::string>("name"), "zhangsan");
    test_p_eq(*cache.data<int>("age"), 25);
    test_p_eq(body->tall, 175);
    test_p_eq(body->weight, 60);
}

sf_test(cache, count_limit_test)
{
    cache cache(2);
    cache.set_data("name", std::string("zhangsan"));
    cache.set_data("age", 25);
    cache.set_data("body", body_data { 175, 60 });
    cache.set_data("name", std::string("zhangsan"));
    test_np_neq(cache.data<std::string>("name"), nullptr);
    test_np_eq(cache.data<int>("age"), nullptr);
    test_np_neq(cache.data<body_data>("body"), nullptr);
}

sf_test(cache, overwrite_test)
{
    cache cache;
    cache.set_data("name", std::string("zhangsan"));
    cache.set_data("name", std::string("lisi"));
    test_p_eq(*cache.data<std::string>("name"), "lisi");
}

sf_test(cache, update_timestamp_test)
{
    cache cache(2);
    cache.set_data("name", std::string("zhangsan"));
    cache.set_data("age", 25);
    cache.data<string>("name");
    cache.set_data("body", body_data { 175, 60 });
    test_np_neq(cache.data<std::string>("name"), nullptr);
    test_np_eq(cache.data<int>("age"), nullptr);
    test_np_neq(cache.data<body_data>("body"), nullptr);
}

sf_test(cache, mismatch_type_test)
{
    cache cache;
    cache.set_data("name", std::string("zhangsan"));
    test_np_eq(cache.data<int>("name"), nullptr);
}

sf_test(coroutine, normal)
{
    int      n = 0;
    co_mutex mu;

    auto co1 = coroutine({ co_attr::set_name("co1") }, [&n, &mu] {
        for (int i = 0; i < 1000; ++i)
        {
            std::lock_guard<co_mutex> lck(mu);
            n += i;
            this_coroutine::yield_coroutine();
        }
    });
    auto co2 = coroutine({ co_attr::set_name("co2") }, [&n, &mu] {
        for (int i = 0; i < 1000; ++i)
        {
            std::lock_guard<co_mutex> lck(mu);
            n += i;
            this_coroutine::yield_coroutine();
        }
    });

    auto co3 = coroutine({ co_attr::set_name("co3") }, [&n, &mu] {
        for (int i = 0; i < 1000; ++i)
        {
            std::lock_guard<co_mutex> lck(mu);
            n += i;
            this_coroutine::yield_coroutine();
        }
    });
    co1.wait();
    co2.wait();
    co3.wait();

    test_p_eq(n, 1498500);
}

sf_test(coroutine, get)
{
    auto co = coroutine({ co_attr::set_name("co") }, []() {
        return 10;
    });
    auto t  = co.get();
    test_p_eq(t, 10);
}

#else

#endif