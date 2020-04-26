
/**
* @version 1.0.0
* @author skyfire
* @file test_sf_logger.cpp
*/

// 1.SF_DEBUG表示启用sf_debug宏
#define SF_DEBUG
#include "tools/sf_logger"

using namespace skyfire;
using namespace std;

int main()
{
    // 2.获取单例对象
    auto logger = sf_logger::instance();
    // 3.添加日志文件，SF_WARN_LEVEL以上的日志级别将会打印到此文件中
    logger->add_level_file(SF_WARN_LEVEL, "runlog.log", sf_default_log_format, false);
    // 4. json格式
    logger->add_level_func(
        SF_DEBUG_LEVEL, [](const sf_logger_info_t& log_info, bool) {
            cout << sf_colored_string(to_json(log_info).to_string(), sf_log_color_map[log_info.level]) << endl;
        },
        false);
    // 5.日志打印
    sf_error("hello", "world");
    sf_warn("this is warn");
    sf_debug("this is debug msg", 666);
    sf_info("this is info");
    sf_fatal("crashed!");
    getchar();
    g_logger->stop_logger();
    getchar();
}