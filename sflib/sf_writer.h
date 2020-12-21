#pragma once

#include "sf_error.h"
#include "sf_type.h"

namespace skyfire
{
class writer
{
public:
    virtual sf_error write(const byte_array& data) = 0;
    virtual bool     can_write() const             = 0;
};

}