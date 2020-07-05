#include <sf_event_waiter>
#include <sf_object>
#include <sf_stdc++>
#include <sf_test>

class test_object: public skyfire::sf_object{
    SF_REG_SIGNAL(s1);
    SF_REG_SIGNAL(s2, int, float, double);
};

bool none_param_event_waiter_test()
{
    test_object t;
    std::thread([&t]() {
        std::this_thread::sleep_for(std::chrono::seconds(5));
        t.s1();
    }).detach();
    SF_WAIT(&t, s1);
    return true;
}

bool many_param_event_waiter_test()
{
    test_object t;
    std::thread([&t]() {
        std::this_thread::sleep_for(std::chrono::seconds(5));
        t.s2(5,6.5,504.2);
    }).detach();
    SF_WAIT(&t, s2);
    return true;
}

int main()
{
    sf_test(none_param_event_waiter_test);
    sf_test(many_param_event_waiter_test);

    sf_test_run();
}