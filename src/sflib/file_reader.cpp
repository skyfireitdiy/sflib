#include "sflib/file_reader.h"
#include "sflib/utils.h"

namespace skyfire
{
file_reader::file_reader(const std::string& filename)
    : fi__(filename, std::ios::binary | std::ios::in)
{
}

byte_array_result file_reader::read(size_t max_size)
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

bool file_reader::can_read() const
{
    return fi__ && !fi__.eof();
}

file_reader::~file_reader()
{
    if (fi__)
    {
        fi__.close();
    }
}
}