#include <iostream>
#define SF_LOG_OUTPUT_THREAD_ID
#define SF_LOGGER_STANDALONE
#include "sf_logger.h"

#include <iostream>
#include <sf_finally.h>

using namespace skyfire;
using namespace std;

void output(const std::string& str)
{
    cout<<"--->"<<str<<flush;
}

int main()
{
    auto logger = sf_logger::get_instance();
    int a = logger->add_level_stream(SF_DEBUG_LEVEL, &cout);
    int b = logger->add_level_func(SF_ERROR_LEVEL, output);
    int c = logger->add_level_file(SF_WARN_LEVEL, "runlog.log");
    getchar();
    logger->sf_error("hello", "world");
    logger->sf_warn("this is warn");
    getchar();
    logger->remove_filter(b);
    logger->sf_error("hello", "world");
    logger->sf_warn("this is warn");
    getchar();
}