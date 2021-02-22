#pragma once

#include "sf_stdc++.h"
#include "sf_nocopy.h"
#include "sf_reader.h"
#include "sf_utils.h"

namespace skyfire
{
class file_reader : public make_instance_t<file_reader, nocopy<reader>>
{
private:
    file_reader(const std::string& filename);

    std::ifstream fi__;

public:
    byte_array_result read(size_t max_size) override;
    bool              can_read() const override;

    ~file_reader();
};
}