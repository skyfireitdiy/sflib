#pragma once
#include "sflib/core/type.h"
#include "sflib/tools/error.h"
#include "sflib/tools/multi_value.h"

namespace skyfire
{
using byte_array_result = multi_value<sf_error, byte_array>;
class reader
{
public:
    virtual byte_array_result read(size_t max_size) = 0;
    virtual bool can_read() const = 0;
};
}