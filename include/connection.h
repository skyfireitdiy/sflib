#pragma once

#include "multi_value.h"
#include "reader.h"
#include "utils.h"

namespace skyfire
{
class connection
{
private:
public:
    virtual byte_array_result read(size_t size)             = 0;
    virtual int               write(const byte_array& data) = 0;
    virtual sf_error          close()                       = 0;
};

using connection_result = multi_value<sf_error, std::shared_ptr<connection>>;

connection_result dial(const std::string& addr);

} // namespace skyfire
