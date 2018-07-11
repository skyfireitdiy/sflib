//
// Created by skyfire on 18-7-11.
//

#define SF_DEBUG

#include "sf_event_waiter.h"
#include "sf_timer.h"
#include "sf_logger.h"
#include <iostream>

using namespace std;
using namespace skyfire;


sf_timer timer;

int main()
{
    auto logger = sf_logger ::get_instance();
    logger->add_level_stream(SF_LOG_LEVEL::SF_DEBUG_LEVEL, &cout);
    logger->sf_debug("开始");
    sf_eventloop loop;
    sf_bind_signal(&timer, timeout, [&](){
        loop.quit();
    },false);
    timer.start(5000, true);
    loop.exec();
    logger->sf_debug("结束");
}