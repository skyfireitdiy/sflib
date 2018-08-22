#include <iostream>
#define SF_LOG_OUTPUT_THREAD_ID
#define SF_LOGGER_STANDALONE
#include "sf_logger.hpp"

#include <iostream>
#include <sf_finally.h>

using namespace skyfire;


void output(const std::string& str)
{
    std::cout<<"--->"<<str<<std::flush;
}

int main()
{
    auto logger = sf_logger::get_instance();
    int a = logger->add_level_stream(SF_DEBUG_LEVEL, &std::cout);
    int b = logger->add_level_func(SF_ERROR_LEVEL, output);
    int c = logger->add_level_file(SF_WARN_LEVEL, "runlog.log");
    getchar();
    sf_error("hello", "world");
    sf_warn("this is warn");
    getchar();
    logger->remove_filter(b);
    sf_error("hello", "world");
    sf_warn("this is warn");
    getchar();
}