#pragma once

#include "reader.h"
#include "utils.h"
namespace skyfire
{
class connector : public make_instance_t<connector>
{
private:
public:
    sf_error          connect(const std::string& addr);
    byte_array_result read(size_t size);
    int               write(const byte_array& data);
    sf_error          close();
};
} // namespace skyfire
