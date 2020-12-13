#include <sf_finally>
#include <sf_stdc++>
#include <sf_test>

using namespace skyfire;
using namespace std;

sf_test(test_finally_order)
{
    int a = 10;
    {
        sf_finally([&]() {
            a *= 10;
        });
        {
            sf_finally([&]() {
                a += 10;
            });
        }
    }
    test_p_eq(a , 200);
}

sf_test(test_finally_order2)
{
    int a = 10;
    {
        sf_finally([&]() {
            a *= 10;
        });
        sf_finally([&]() {
            a += 10;
        });
    }
    test_p_eq(a , 200);
}

int main()
{
    run_test();
}