
/**
* @version 1.0.0
* @author skyfire
* @email skyfireitdiy@hotmail.com
* @see http://github.com/skyfireitdiy/sflib
* @file test_sf_logger.cpp

* sflib第一版本发布
* 版本号1.0.0
* 发布日期：2018-10-22
*/

#include <iostream>
#define SF_LOGGER_STANDALONE
#include "sf_logger.hpp"

#include <iostream>
#include <sf_finally.h>

using namespace skyfire;



int main()
{
    auto logger = sf_logger::get_instance();
    int c = logger->add_level_file(SF_WARN_LEVEL, "runlog.log");
    sf_error("hello", "world");
    sf_warn("this is warn");
    sf_error("hello", "world");
    sf_warn("this is warn");
    getchar();
    g_logger->stop_logger();
    getchar();
}