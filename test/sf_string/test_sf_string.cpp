#include <sf_string>
#include <sf_test>

using namespace skyfire;

bool test_sf_string_repeat()
{
    return string::repeat("1", 6) == string("111111");
}

bool test_sf_string_operator()
{
    return string("h") * 5 == "hhhhh";
}

bool test_sf_string_output()
{
    return (std::cout << string::repeat("hello", 5) << std::endl).good();
}

int main()
{
    sf_test_add(test_sf_string_repeat);
    sf_test_add(test_sf_string_output);
    sf_test_add(test_sf_string_operator);

    test_run();
}