#include "io_fd.h"
namespace skyfire
{
byte_array_result io_fd::read(size_t max_size)
{
    // TODO 读取
    return {};
}
bool     io_fd::can_read() const { return readable__; }
sf_error io_fd::write(const byte_array& data)
{
    // TODO 写入
    return {};
}
bool io_fd::can_write() const { return writeable__; }
}