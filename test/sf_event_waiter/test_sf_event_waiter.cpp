#include <sf_event_waiter>
#include <sf_object>
#include <sf_stdc++>
#include <sf_test>

class test_object : public skyfire::object
{
    sf_singal(s1);
    sf_singal(s2, int, float, double);
};

sf_test(none_param_event_waiter_test)
{
    test_object t;
    std::thread([&t]() {
        std::this_thread::sleep_for(std::chrono::seconds(5));
        t.s1();
    }).detach();
    sf_wait(&t, s1);
}

sf_test(many_param_event_waiter_test)
{
    test_object t;
    std::thread([&t]() {
        std::this_thread::sleep_for(std::chrono::seconds(5));
        t.s2(5, 6.5, 504.2);
    }).detach();
    sf_wait(&t, s2);
}

int main()
{
    skyfire::run_test();
}