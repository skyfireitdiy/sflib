
#include "sflib/tcp_utils.h"
namespace skyfire
{

void make_header_checksum(pkg_header_t& header)
{
    header.checksum = 0;
    auto offset     = SF_GET_OFFSET(pkg_header_t, type);
    auto p_byte     = reinterpret_cast<const unsigned char*>(&header);
    for (auto i = offset; i < sizeof(header); ++i)
    {
        header.checksum ^= p_byte[i];
    }
}
bool check_header_checksum(const pkg_header_t& header)
{
    unsigned char checksum = 0;
    auto          offset   = SF_GET_OFFSET(pkg_header_t, type);
    auto          p_byte   = reinterpret_cast<const unsigned char*>(&header);
    for (auto i = offset; i < sizeof(header); ++i)
    {
        checksum ^= p_byte[i];
    }
    return checksum == header.checksum;
}
unsigned long long ntoh64(unsigned long long input)
{
    if (big_endian())
    {
        return input;
    }
    unsigned long long val;
    auto*              data = reinterpret_cast<unsigned char*>(&val);
    data[0]                 = static_cast<unsigned char>(input >> 56);
    data[1]                 = static_cast<unsigned char>(input >> 48);
    data[2]                 = static_cast<unsigned char>(input >> 40);
    data[3]                 = static_cast<unsigned char>(input >> 32);
    data[4]                 = static_cast<unsigned char>(input >> 24);
    data[5]                 = static_cast<unsigned char>(input >> 16);
    data[6]                 = static_cast<unsigned char>(input >> 8);
    data[7]                 = static_cast<unsigned char>(input >> 0);
    return val;
}
unsigned long long hton64(unsigned long long input)
{
    return (ntoh64(input));
}
bool big_endian()
{
    char data[4] = { 0x12, 0x34, 0x56, 0x78 };
    return *reinterpret_cast<int*>(data) == 0x12345678;
}
} // namespace skyfire
