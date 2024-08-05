
#include "sflib/http/websocket_utils.h"
namespace skyfire
{
unsigned long long get_size(
    const websocket_client_data_1_header_t &header)
{
    return static_cast<unsigned long long int>(header.mask_len & 0b01111111);
    ;
}
void decode_websocket_pkg(byte_array &data,
                          const unsigned char *mask_key)
{
    auto size = data.size();
    for (size_t i = 0; i < size; ++i)
    {
        data[i] ^= mask_key[i % 4];
    }
}
unsigned long long get_size(
    const websocket_client_data_2_header_t &header)
{
    return ntohs(*reinterpret_cast<const unsigned short *>(header.extend_len));
}

unsigned long long get_size(
    const websocket_client_data_3_header_t &header)
{
    return ntoh64(
        *reinterpret_cast<const unsigned long long *>(header.extend_len));
}
} // namespace skyfire
