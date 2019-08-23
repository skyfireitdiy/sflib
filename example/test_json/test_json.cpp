#include "tools/sf_json.hpp"

using namespace skyfire;

struct Time {
    int h;
    int m;
    int s;
};

SF_JSONIFY(Time, h, m, s)

struct People {
    int age;
    std::string name;
    std::string address;
    Time t;
};

SF_JSONIFY(People, age, name, address, t)

int main() {
    using namespace std::literals;
    // 1. 使用字面值操作符解析json
    auto json1 = R"({"name": "wmb","data": [1,2,3,"666"]})"_json;
    auto json2 = R"({"age":25, "address":"西安"})"_json;
    auto json3 = R"({"company":"saming"})"_json;
    // 2. 输出json
    std::cout << json1 << std::endl;
    std::cout << json2 << std::endl;
    std::cout << json3 << std::endl;
    std::cout << "---------" << std::endl;
    // 3. 合并json
    json1.join(json2);
    json1.join(json3.deep_copy());
    std::cout << json1 << std::endl;
    std::cout << json2 << std::endl;
    std::cout << json3 << std::endl;
    std::cout << "---------" << std::endl;
    // 4.
    // sf_json底层使用指针，数据属于浅拷贝，所以改变json2的值同样会影响json1的值
    json2["address"] = "北京";
    std::cout << json1 << std::endl;
    std::cout << json2 << std::endl;
    std::cout << json3 << std::endl;
    std::cout << "---------" << std::endl;
    // 5.
    // json3与json1进行合并时，使用deep_copy()创建了副本（深拷贝）,操作json3不会影响json1
    json3["company"] = "lenovo";
    std::cout << json1 << std::endl;
    std::cout << json2 << std::endl;
    std::cout << json3 << std::endl;
    std::cout << "---------" << std::endl;

    // 6.json序列化
    std::tuple<int, std::string, double> t{5, "hello"s, 5.2};
    auto t_js = to_json(t);
    std::cout << t_js << std::endl;

    std::tuple<int, std::string, double> t2;
    from_json(t_js, t2);
    std::cout << std::get<0>(t2) << std::endl;
    std::cout << std::get<1>(t2) << std::endl;
    std::cout << std::get<2>(t2) << std::endl;

    People p;
    p.name = "skyfire";
    p.age = 100;
    p.address = "China";
    p.t.h = 24;
    p.t.m = 60;
    p.t.s = 25;
    auto js = to_json(p);
    std::cout << js << std::endl;

    People p2;
    from_json(js, p2);
    std::cout << to_json(p2) << std::endl;

    // 7.对字节容器的序列化有优化
    byte_array bin_data{1,  2,  3,  4,  5,  6,  7,  8,  9,  10, 11, 12, 13,
                        14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26};
    auto bin_data_json = to_json(bin_data);
    std::cout << bin_data_json << std::endl;

    byte_array bin_data2;
    from_json(bin_data_json, bin_data2);
    for (auto &p : bin_data2) {
        std::cout << int(p) << std::ends;
    }
    std::cout << std::endl;

    getchar();
}
