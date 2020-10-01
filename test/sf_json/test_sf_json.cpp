#include <sf_json>
#include <sf_stdc++>
#include <sf_test>

using namespace skyfire;
using namespace std;

sf_test(test_construct)
{
    sf_json js = 5;
    sf_test_num_eq(static_cast<int>(js), 5);
    js = 6.5;
    sf_test_num_eq(static_cast<double>(js), 6.5);
    js = "hello world"s;
    sf_test_str_eq(static_cast<string>(js), "hello world");
    js = true;
    sf_test_assert(js);
    js = false;
    sf_test_assert(!js);
    return true;
}

sf_test(test_insert)
{
    sf_json js;
    js["name"] = "zhangsan";
    js["age"] = 30;
    js["man"] = false;
    js["address"] = sf_json {};

    return js["address"].is_null();
}

sf_test(test_compare)
{
    sf_json js1;
    js1["name"] = "zhangsan";
    js1["age"] = 30;

    sf_json js2;
    js2["name"] = "zhangsan";
    js2["age"] = 30;

    sf_test_p_eq(js1, js2);

    // at 不会新增元素
    sf_test_p_eq(js1.at("unknown"), sf_json {});

    sf_test_p_eq(js1, js2);

    // []会新增元素
    sf_test_p_eq(js1["unknown"], sf_json {});

    sf_test_p_neq(js1, js2);

    return true;
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

    sf_test_str_eq(std::string(js.at("name")), "王小二"s);
    sf_test_num_eq(int(js.at("age")), 25);
    sf_test_str_eq(std::string(js.at("birthday")), "1990-01-01");
    sf_test_str_eq(std::string(js.at("school")), "蓝翔");
    sf_test_str_eq(std::string(js.at("major").at(0)), "理发");
    sf_test_str_eq(std::string(js.at("major").at(1)), "挖掘机");
    sf_test_assert(!js.at("has_girlfriend"));
    sf_test_assert(js.at("car").is_null());
    sf_test_assert(js.at("house").is_null());

    return true;
}

sf_test(test_convert)
{
    sf_json js;
    js.convert_to_object();
    sf_test_np_eq(js.type(), sf_json_type::object);
    js.convert_to_array();
    sf_test_np_eq(js.type(), sf_json_type::array);
    js.convert_to_null();
    sf_test_assert(js.is_null());

    return true;
}

int main()
{
    return sf_test_run(4);
}