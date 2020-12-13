#include <sf_string>
#include <sf_test>

using namespace skyfire;

sf_test(test_sf_string_repeat)
{
    test_p_eq(string::repeat("1", 6), string("111111"));
}

sf_test(test_sf_string_operator)
{
    test_p_eq(string("h") * 5, "hhhhh");
}

sf_test(test_sf_string_output)
{
    test_assert((std::cout << string::repeat("hello", 5) << std::endl).good());
}

int main()
{
    run_test();
}