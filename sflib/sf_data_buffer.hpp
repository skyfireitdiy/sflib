#pragma once

#include "sf_data_buffer.h"

namespace skyfire {
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

err data_buffer::read(int max_size, byte_array& data)
{
    std::lock_guard<std::mutex> lock(mutex__);

    if (data__.empty() && (reader__ == nullptr || !reader__->can_read())) {
        return err { { err_finished, "read finished" } };
    }

    data.resize(max_size);

    if (max_size <= data__.size()) {
        memcpy(data.data(), data__.data(), max_size);
        memcpy(data__.data(), data__.data() + max_size, data__.size() - max_size);
        data__.resize(data__.size() - max_size);
        return err {};
    }

    data = data__;
    data__.clear();

    if (reader__ == nullptr || !reader__->can_read()) {
        return err {};
    }

    byte_array tmp_data;
    auto e = reader__->read(max_size - data.size(), tmp_data);
    if (!e) {
        return e;
    }

    data += tmp_data;
    return err {};
}

err data_buffer::write(const byte_array& data)
{
    std::lock_guard<std::mutex> lock(mutex__);

    if (writer__ == nullptr || !writer__->can_write()) {
        data__ += data;
        return err {};
    }

    if (!data__.empty()) {
        auto e = writer__->write(data__);
        if (!e) {
            return e;
        }
        data__.clear();
    }

    auto e = writer__->write(data);
    if (!e) {
        return e;
    }
    return err {};
}

bool data_buffer::can_read() const
{
    std::lock_guard<std::mutex> lock(mutex__);
    if (reader__ != nullptr && reader__->can_read()) {
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
