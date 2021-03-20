#pragma once
#include "error.h"
#include "multi_value.h"
#include "type.h"
namespace skyfire
{
using byte_array_result = multi_value<sf_error, byte_array>;
class reader
{
public:
    virtual byte_array_result read(size_t max_size) = 0;
    virtual bool              can_read() const      = 0;
};
}