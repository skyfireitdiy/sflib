#include "sf_data_buffer"
#include "sf_error"
#include "sf_logger"
#include "sf_test"

sf_test(test_read_write)
{
    skyfire::data_buffer buffer;
    auto                 data = skyfire::to_byte_array("hello world");
    buffer.write(data);
    auto t = buffer.read(1024);
    test_np_eq(data, std::get<1>(t));
}

sf_test(test_read_empty_buffer)
{
    skyfire::data_buffer buffer;
    auto                 t = buffer.read(1024);
    test_np_eq((skyfire::sf_error { { skyfire::err_finished, "" } }), std::get<0>(t));
}

sf_test(test_read_twice)
{
    skyfire::data_buffer buffer;
    buffer.set_data(skyfire::to_byte_array("hello world"));
    auto t = buffer.read(5);
    test_assert(std::get<0>(t));
    test_num_eq(std::get<1>(t).size(), 5U);
    t = buffer.read(10);
    test_num_eq(skyfire::to_string(t).length(), 6U);
    test_str_eq(skyfire::to_string(t), " world");
}

sf_test(test_pipe)
{
    auto reader = skyfire::data_buffer::make_instance();
    auto writer = skyfire::data_buffer::make_instance();
    auto pipe   = skyfire::data_buffer::make_instance();

    pipe->set_reader(writer);
    reader->set_reader(pipe);

    writer->write(skyfire::to_byte_array("hello world"));
    auto t = reader->read(1024);

    using namespace std::string_literals;

    test_str_eq(std::get<1>(t).data(), "hello world"s);
}

int main()
{
    return skyfire::run_test();
}