#pragma once

#include "sf_data_buffer.h"
#include "sf_type.hpp"

namespace skyfire
{

void data_buffer::set_data(const byte_array& data)
{
    std::lock_guard<std::mutex> lock(mutex__);
    data__ = data;
}

void data_buffer::clear()
{
    std::lock_guard<std::mutex> lock(mutex__);
    data__.clear();
}

byte_array_result data_buffer::read(size_t max_size)
{
    std::lock_guard<std::mutex> lock(mutex__);

    if (data__.empty() && (reader__ == nullptr || !reader__->can_read()))
    {
        return { sf_error { { err_finished, "read finished" } }, byte_array {} };
    }

    byte_array data(max_size);

    if (max_size <= data__.size())
    {
        memcpy(data.data(), data__.data(), max_size);
        memcpy(data__.data(), data__.data() + max_size, data__.size() - max_size);
        data__.resize(data__.size() - max_size);
        return { sf_error {}, data };
    }

    data = data__;
    data__.clear();

    if (reader__ == nullptr || !reader__->can_read())
    {
        return { sf_error {}, data };
    }

    byte_array tmp_data;
    auto       e = reader__->read(max_size - data.size());
    if (!std::get<0>(e))
    {
        return e;
    }

    data += e;
    return { sf_error {}, data };
}

sf_error data_buffer::write(const byte_array& data)
{
    std::lock_guard<std::mutex> lock(mutex__);

    data__ += data;

    if (writer__ == nullptr || !writer__->can_write())
    {
        return sf_error {};
    }

    auto e = writer__->write(data);
    if (!e)
    {
        return e;
    }
    return sf_error {};
}

bool data_buffer::can_read() const
{
    std::lock_guard<std::mutex> lock(mutex__);
    if (reader__ != nullptr && reader__->can_read())
    {
        return true;
    }

    return !data__.empty();
}

bool data_buffer::can_write() const
{
    return true;
}

void data_buffer::set_reader(std::shared_ptr<reader> rdr)
{
    std::lock_guard<std::mutex> lock(mutex__);
    reader__ = rdr;
}

void data_buffer::set_writer(std::shared_ptr<writer> wtr)
{
    std::lock_guard<std::mutex> lock(mutex__);
    writer__ = wtr;
}

}
