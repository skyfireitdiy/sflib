#define SF_DEBUG
#include <sf_cache>
#include <sf_logger>
#include <sf_stdc++>
#include <sf_test>

using namespace skyfire;

struct body_data
{
    int tall;
    int weight;
};

sf_test(normal_test)
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

sf_test(count_limit_test)
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

sf_test(overwrite_test)
{
    cache cache;
    cache.set_data("name", std::string("zhangsan"));
    cache.set_data("name", std::string("lisi"));
    test_p_eq(*cache.data<std::string>("name"), "lisi");
}

sf_test(update_timestamp_test)
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

sf_test(mismatch_type_test)
{
    cache cache;
    cache.set_data("name", std::string("zhangsan"));
    test_np_eq(cache.data<int>("name"), nullptr);
}

int main()
{
    run_test();
}