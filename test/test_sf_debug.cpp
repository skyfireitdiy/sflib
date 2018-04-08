#include <iostream>
#include "sf_debug.h"

using namespace skyfire;

class my_debug : public sf_debug<>
{
protected:
    void out_debug_string_(const char *str)
    {
        std::cout<< str << std::endl;
    }
};


int main()
{
    my_debug debug;
    debug.sf_track("666", 123);
}