#pragma once

#include "sf_file_reader.h"
#include "sf_utils.h"

namespace skyfire
{
inline file_reader::file_reader(const std::string& filename)
    : fi__(filename, std::ios::binary | std::ios::in)
{
}

inline byte_array_result file_reader::read(size_t max_size)
{
    if (!fi__)
    {
        return { { err_stream, "" }, {} };
    }
    if (fi__.eof())
    {
        return { { err_finished, "" }, {} };
    }
    byte_array data(max_size);
    if (fi__.read(data.data(), max_size))
    {
        data.resize(fi__.gcount());
        return { {}, data };
    }
    else
    {
        return { { err_stream, "" }, {} };
    }
}

inline bool file_reader::can_read() const
{
    return fi__ && !fi__.eof();
}

inline file_reader::~file_reader()
{
    if (fi__)
    {
        fi__.close();
    }
}
}