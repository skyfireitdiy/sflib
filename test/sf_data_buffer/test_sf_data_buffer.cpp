#include "sf_data_buffer"
#include "sf_test"

sf_test(test_read_write)
{
    skyfire::data_buffer buffer;
    auto data = skyfire::to_byte_array("hello world");
    buffer.write(data);
    skyfire::byte_array t;
    buffer.read(1024, t);
    return t == data;
}

int main()
{
    return skyfire::test_run();
}