
/**
* @version 1.0.0
* @author skyfire
* @file test_event_waiter.cpp
*/

//
// Created by skyfire on 18-7-11.
//

#include "sf_event_waiter"
#include "sf_object"
#include "sf_timer"

using namespace skyfire;

sf_timer timer;

int main() {
    std::cout << "Start" << std::endl;
    timer.start(5000, true);
    // 1. 等待超时
    sf_wait(&timer, timeout);
    std::cout << "End" << std::endl;
}