#include <iostream>
//#define SF_DISABLE_DEBUG
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
    logger->add_level_stream(SF_DEBUG_LEVEL, &cout);
    logger->add_level_func(SF_ERROR_LEVEL, output);
    getchar();
    logger->sf_error("hello", "world");
    logger->sf_warn("this is warn");
    getchar();
}