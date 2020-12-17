#include <sf_json>
#include <sf_stdc++>
#include <sf_test>

using namespace skyfire;
using namespace std;

sf_test(test_construct)
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

sf_test(test_insert)
{
    json js;
    js["name"]    = "zhangsan";
    js["age"]     = 30;
    js["man"]     = false;
    js["address"] = json {};

    test_assert(js["address"].is_null());
}

sf_test(test_compare)
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

sf_test(test_parse)
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

sf_test(test_convert)
{
    json js;
    js.convert_to_object();
    test_np_eq(js.type(), json_type::object);
    js.convert_to_array();
    test_np_eq(js.type(), json_type::array);
    js.convert_to_null();
    test_assert(js.is_null());
}

int main()
{
    return run_test(4);
}