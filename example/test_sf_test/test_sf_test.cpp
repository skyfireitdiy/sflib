#include <sf_test>

using namespace std;

int func1(int a, int b)
{
    if (b == 0) {
        throw std::runtime_error("b == 0");
    }
    return a / b;
}

bool test1()
{
    return false;
}
bool test2()
{
    return true;
}

int main()
{
    sf_assert(1 + 1 == 2, "add error");
    sf_should_no_throw(func1(2, 4), "should ok");
    sf_should_no_throw(func1(2, 4), "should ok");
    sf_should_no_throw(func1(2, 0), "ddd");

    sf_test_func(test1);
    sf_test_func(test2);
    skyfire::sf_test::run();
}
