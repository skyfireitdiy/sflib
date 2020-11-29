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

bool normal_test()
{
    cache cache;
    cache.set_data("name", "zhangsan");
    cache.set_data("age", 25);
    cache.set_data("body", body_data { 175, 60 });

    auto body = cache.data<body_data>("body");

    return *cache.data<std::string>("name") == "zhangsan" && *cache.data<int>("age") == 25 && body->tall == 175 && body->weight == 60;
}

bool count_limit_test()
{
    cache cache(2);
    cache.set_data("name", std::string("zhangsan"));
    cache.set_data("age", 25);
    cache.set_data("body", body_data { 175, 60 });
    cache.set_data("name", std::string("zhangsan"));

    return cache.data<string>("name") != nullptr && cache.data<int>("age") == nullptr && cache.data<body_data>("body") != nullptr;
}

bool overwrite_test()
{
    cache cache;
    cache.set_data("name", std::string("zhangsan"));
    cache.set_data("name", std::string("lisi"));
    return *cache.data<std::string>("name") == "lisi";
}

bool update_timestamp_test()
{
    cache cache(2);
    cache.set_data("name", std::string("zhangsan"));
    cache.set_data("age", 25);
    cache.data<string>("name");
    cache.set_data("body", body_data { 175, 60 });

    return cache.data<string>("name") != nullptr && cache.data<int>("age") == nullptr && cache.data<body_data>("body") != nullptr;
}

bool mismatch_type_test()
{
    cache cache;
    cache.set_data("name", std::string("zhangsan"));
    return cache.data<int>("name") == nullptr;
}

int main()
{
    sf_test_add(normal_test);
    sf_test_add(count_limit_test);
    sf_test_add(overwrite_test);
    sf_test_add(update_timestamp_test);
    sf_test_add(mismatch_type_test);

    run_test(4);
}