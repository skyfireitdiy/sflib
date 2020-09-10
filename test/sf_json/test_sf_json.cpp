#include <sf_json>
#include <sf_stdc++>
#include <sf_test>

using namespace skyfire;
using namespace std;

bool test_construct()
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

bool test_insert()
{
    sf_json js;
    js["name"] = "zhangsan";
    js["age"] = 30;
    js["man"] = false;
    js["address"] = sf_json{};

    return js["address"].is_null();
}

bool test_compare(){
    sf_json js1;
    js1["name"] = "zhangsan";
    js1["age"] = 30;

    sf_json js2;
    js2["name"] = "zhangsan";
    js2["age"] = 30;

    sf_test_p_eq(js1, js2);

    // at 不会新增元素
    if (js1.at("unknown") == sf_json{}){

    }

    sf_test_p_eq(js1, js2);  

    return true;
}

int main()
{
    sf_test(test_construct);
    sf_test(test_insert);
    sf_test(test_compare);

    return sf_test_run();
}