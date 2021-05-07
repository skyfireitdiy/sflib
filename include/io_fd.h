#pragma once
#include "object.h"
#include "reader.h"
#include "utils.h"
#include "writer.h"
namespace skyfire
{
class io_fd : public reader, public writer, public make_instance_t<io_fd>
{
private:
    int      fd__;
    sf_error err__;

protected:
    io_fd(int fd);

public:
    virtual byte_array_result read(size_t max_size) override;
    virtual bool              can_read() const override;
    virtual sf_error          write(const byte_array& data) override;
    virtual bool              can_write() const override;
};
}