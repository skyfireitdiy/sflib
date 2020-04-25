#include <core/sf_string>
#include <tools/sf_test>

using namespace skyfire;

bool test_sf_string_repeat()
{
    return sf_string::repeat("1", 6) == sf_string("111111");
}

bool test_sf_string_output()
{
    return (std::cout << sf_string::repeat("hello", 5) << std::endl).good();
}

int main()
{
    sf_test_func(test_sf_string_repeat);
    sf_test_func(test_sf_string_output);

    sf_test::run();
}