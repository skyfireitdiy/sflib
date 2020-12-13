#include <sf_multi_value>
#include <sf_test>

using namespace skyfire;

sf_test(test_multi_value)
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

int main()
{
    run_test();
}