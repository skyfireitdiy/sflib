#include "sf_data_buffer"
#include "sf_error"
#include "sf_test"

sf_test(test_read_write)
{
    skyfire::data_buffer buffer;
    auto data = skyfire::to_byte_array("hello world");
    buffer.write(data);
    auto t = buffer.read(1024);
    return data == std::get<1>(t);
}

sf_test(test_read_empty_buffer)
{
    skyfire::data_buffer buffer;
    auto t = buffer.read(1024);
    return skyfire::err { { skyfire::err_finished, "" } } == std::get<0>(t);
}

sf_test(test_read_twice)
{
    skyfire::data_buffer buffer;
    buffer.set_data(skyfire::to_byte_array("hello world"));
    auto t = buffer.read(5);
    test_assert(std::get<0>(t));
    test_num_eq(std::get<1>(t).size(), 5);
    t = buffer.read(10);
    test_num_eq(skyfire::to_string(t).length(), 6);
    test_str_eq(skyfire::to_string(t), " world");
    return true;
}

int main()
{
    return skyfire::test_run();
}