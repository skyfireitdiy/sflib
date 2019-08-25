
/**
* @version 1.0.0
* @author skyfire
* @mail skyfireitdiy@hotmail.com
* @see http://github.com/skyfireitdiy/sflib
* @file test_event_waiter.cpp

* sflib第一版本发布
* 版本号1.0.0
* 发布日期：2018-10-22
*/

//
// Created by skyfire on 18-7-11.
//

#include "core/sf_event_waiter.hpp"
#include "core/sf_object.hpp"
#include "core/sf_timer.hpp"

using namespace skyfire;

sf_timer timer;

int main() {
    std::cout << "Start" << std::endl;
    timer.start(5000, true);
    // 1. 等待超时
    SF_WAIT(&timer, timeout);
    std::cout << "End" << std::endl;
}