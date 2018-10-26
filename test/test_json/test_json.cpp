#include <sf_json.h>
#include "sf_json.hpp"
#include <iostream>

using namespace skyfire;
using namespace std;

int main()
{
    sf_json json = sf_json::from_string(R"({"hello":"world","data":[1,2,3,"666"]})");
    cout << (json.type() == sf_json_type::null) << endl;
    cout << (std::string) json["hello"] << std::endl;
}