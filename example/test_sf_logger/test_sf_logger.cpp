
/**
* @version 1.0.0
* @author skyfire
* @file test_sf_logger.cpp
*/

// 1.SF_DEBUG表示启用sf_debug宏
#define SF_DEBUG
#include "tools/sf_logger.hpp"

using namespace skyfire;

int main() {
    // 2.获取单例对象
    auto logger = sf_logger::instance();
    // 3.添加日志文件，SF_WARN_LEVEL以上的日志级别将会打印到此文件中
    logger->add_level_file(SF_WARN_LEVEL, "runlog.log");
    // 4.SF_INFO_LEVEL以上的日志级别将会打印到标准输出流
    logger->add_level_stream(SF_INFO_LEVEL, &std::cout);
    // 5.日志打印
    sf_error("hello", "world");
    sf_warn("this is warn");
    sf_error("hello", "world");
    sf_warn("this is warn");
    getchar();
    g_logger->stop_logger();
    getchar();
}