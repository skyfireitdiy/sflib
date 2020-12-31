#pragma once
#include "sf_error.h"
#include "sf_multi_value.h"
#include "sf_reader.h"
#include "sf_stdc++.h"
#include "sf_type.h"
#include "sf_utils.h"
#include "sf_writer.h"
namespace skyfire
{
class data_buffer : public reader, public writer, public make_instance_t<data_buffer>, public std::enable_shared_from_this<data_buffer>
{
private:
    byte_array              data__;
    std::shared_ptr<reader> reader__ = nullptr;
    std::shared_ptr<writer> writer__ = nullptr;
    mutable std::mutex      mutex__;

public:
    data_buffer() = default;
    virtual ~data_buffer();
    void              set_data(const byte_array& data);
    void              clear();
    byte_array_result read(size_t max_size) override;
    sf_error          write(const byte_array& data) override;
    bool              can_read() const override;
    bool              can_write() const override;
    void              set_reader(std::shared_ptr<reader> rdr);
    void              set_writer(std::shared_ptr<writer> wtr);
};
}