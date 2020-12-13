#define SF_DEBUG
#include <sf_chan>
#include <sf_json>
#include <sf_logger>
#include <sf_stdc++>
#include <sf_test>

using namespace skyfire;
using namespace std;

sf_test(test_chan_order)
{
    auto             ch = chan<int>::make_instance();
    std::vector<int> data;
    std::vector<int> result { 1, 2, 3, 4, 5 };
    auto             th1 = std::thread([&ch] {
        for (int i = 0; i < 5; ++i)
        {
            (i + 1) >> ch;
        }
    });
    auto             th2 = std::thread([&data, &ch] {
        for (int i = 0; i < 5; ++i)
        {
            int t = 100;
            ch >> t;
            data.push_back(t);
        }
    });

    th1.join();
    th2.join();
    test_np_eq(data, result);
}

sf_test(test_chan_order_with_buffer)
{
    auto             ch = chan<int>::make_instance(5);
    std::vector<int> data;
    std::vector<int> result { 1, 2, 3, 4, 5 };
    auto             th1 = std::thread([&ch] {
        for (int i = 0; i < 5; ++i)
        {
            (i + 1) >> ch;
        }
    });
    auto             th2 = std::thread([&data, &ch] {
        for (int i = 0; i < 5; ++i)
        {
            int t = 100;
            ch >> t;
            data.push_back(t);
        }
    });

    th1.join();
    th2.join();
    test_np_eq(data, result);
}

sf_test(test_write_to_closed_chan)
{
    auto   ch  = chan<int>::make_instance(5);
    auto   ret = true;
    thread th1([ch]() {
        ch->close();
    });

    thread th2([ch, &ret]() {
        this_thread::sleep_for(chrono::seconds(3));

        ret = 5 >> ch;
    });
    th1.join();
    th2.join();
    test_assert(!ret);
}

sf_test(test_read_from_closed_chan)
{
    auto   ch  = chan<int>::make_instance(5);
    auto   ret = true;
    thread th1([ch, &ret]() {
        this_thread::sleep_for(chrono::seconds(3));
        int t;
        ret = ch >> t;
    });

    thread th2([ch]() {
        5 >> ch;
        ch->close();
    });
    th1.join();
    th2.join();
    test_assert(!ret);
}

int main()
{
    run_test(4);
}