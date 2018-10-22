
/**
* @version 1.0.0
* @author skyfire
* @email skyfireitdiy@hotmail.com
* @see http://github.com/skyfireitdiy/sflib
* @file test_event_waiter.cpp

* sflib第一版本发布
* 版本号1.0.0
* 发布日期：2018-10-22
*/

//
// Created by skyfire on 18-7-11.
//

#define SF_DEBUG

#include "sf_event_waiter.hpp"
#include "sf_timer.hpp"
#include "sf_logger.hpp"
#include <iostream>

using namespace skyfire;


sf_timer timer;

int main()
{
    g_logger->add_level_stream(SF_LOG_LEVEL::SF_DEBUG_LEVEL, &std::cout);
    sf_debug("开始");
    sf_eventloop loop;
    sf_bind_signal(&timer, timeout, [&](){
        loop.quit();
    },false);
    timer.start(5000, true);
    loop.exec();
    sf_debug("结束");
}