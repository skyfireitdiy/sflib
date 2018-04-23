#include <iostream>
//#define SF_DISABLE_DEBUG
#include "sf_debug.h"

#include <iostream>

using namespace skyfire;
using namespace std;

void output(const char *str)
{
    cout<<str<<endl;
}

int main()
{
    sf_debug_t ddd;
    sf_debug_t::set_output_debug_string_func(output);
    ddd.sf_track("666", 123);
    sf_log("123",8997);
}