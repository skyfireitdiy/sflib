#include "sf_serialize.h"

skyfire::byte_array skyfire::operator+(skyfire::byte_array b1,const skyfire::byte_array &b2)
{
    b1.insert(b1.end(),b2.begin(),b2.end());
    return b1;
}


skyfire::byte_array& skyfire::operator+=(skyfire::byte_array &b1,const skyfire::byte_array &b2)
{
    b1.insert(b1.end(),b2.begin(),b2.end());
    return b1;
}