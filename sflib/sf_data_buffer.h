#pragma once

#include "sf_error.h"
#include "sf_stdc++.h"
#include "sf_type.h"

namespace skyfire {

class reader {
public:
    virtual err read(int max_size, byte_array& data) = 0;
    virtual bool can_read() const = 0;
};

class writer {
public:
    virtual err write(const byte_array& data) = 0;
    virtual bool can_write() const = 0;
};

class data_buffer : public reader, writer {
private:
    byte_array data__;
    std::shared_ptr<reader> reader__ = nullptr;
    std::shared_ptr<writer> writer__ = nullptr;
    mutable std::mutex mutex__;

public:
    void set_data(const byte_array& data);
    void clear();
    err read(int max_size, byte_array& data) override;
    err write(const byte_array& data) override;
    bool can_read() const override;
    bool can_write() const override;
    void set_reader(std::shared_ptr<reader> rdr);
    void set_writer(std::shared_ptr<writer> wtr);
};
}