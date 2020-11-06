#include "sf_data_buffer"
#include "sf_test"

sf_test(test_read_write)
{
    skyfire::data_buffer buffer;
    auto data = skyfire::to_byte_array("hello world");
    buffer.write(data);
    auto t  = buffer.read(1024);
    return data == std::get<1>(t);
}

int main()
{
    return skyfire::test_run();
}