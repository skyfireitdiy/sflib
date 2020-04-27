#include <sf_chan>
#include <sf_stdc++>
#include <sf_json>
#include <sf_test>

using namespace skyfire;

bool test_chan_order()
{
    auto ch = sf_chan<int>::make_instance();
    std::vector<int> data;
    std::vector<int> result { 1, 2, 3, 4, 5 };
    auto th1 = std::thread([&ch] {
        for (int i = 0; i < 5; ++i) {
            i + 1 >> ch;
        }
    });
    auto th2 = std::thread([&data, &ch] {
        for (int i = 0; i < 5; ++i) {
            int t = 100;
            ch >> t;
            data.push_back(t);
        }
    });

    th1.join();
    th2.join();
    return data == result;
}

bool test_chan_order_with_buffer()
{
    auto ch = sf_chan<int>::make_instance(5);
    std::vector<int> data;
    std::vector<int> result { 1, 2, 3, 4, 5 };
    auto th1 = std::thread([&ch] {
        for (int i = 0; i < 5; ++i) {
            i + 1 >> ch;
        }
    });
    auto th2 = std::thread([&data, &ch] {
        for (int i = 0; i < 5; ++i) {
            int t = 100;
            ch >> t;
            data.push_back(t);
        }
    });

    th1.join();
    th2.join();
    return data == result;
}

int main()
{
    sf_test_func(test_chan_order);
    sf_test_func(test_chan_order_with_buffer);

    sf_test::run();
}