#include <sf_multi_value>
#include <sf_test>

using namespace skyfire;

bool test_multi_value(){
    const int a = 5;
    const std::string b = "hello world";
    double c = 5.6;
    float d = 9.5f;
    auto e = sf_multi_value(a, b, c, d);
    return a == (int)e && b == (std::string)e && c == (double)e && d == (float)e;
}

int main(){
    sf_test(test_multi_value);
    sf_test_run();
}