#pragma once

#include "stdc++.h"
#include "nocopy.h"
#include "reader.h"
#include "utils.h"

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