
#pragma once
#include "json.h"
#include "tcp_utils.h"
#include "type.h"
#include <arpa/inet.h>
#include <memory>
namespace skyfire
{
#pragma pack(1)
struct websocket_client_data_1_header_t
{
    // 0 bit: FIN标志
    // 1-3 bit: 3位保留
    // 4-7 bit: 操作码
    unsigned char fin_rsv_oc;
    // 0 bit: 掩码
    // 1-7 bit: 数据长度
    // 如果len的值在0-125，则是payload的真实长度。
    unsigned char mask_len;
    // 掩码密钥（当mask为1时有效）
    unsigned char mask_key[4];
};
struct websocket_client_data_2_header_t
{
    // 0 bit: FIN标志
    // 1-3 bit: 3位保留
    // 4-7 bit: 操作码
    unsigned char fin_rsv_oc;
    // 0 bit: 掩码
    // 1-7 bit: 数据长度
    unsigned char mask_len;
    // 如果len的是126，则后面2个字节形成的16bits无符号整型数的值是payload的真实长度。注意，网络字节序，需要转换。
    unsigned char extend_len[2];
    // 掩码密钥（当mask为1时有效）
    unsigned char mask_key[4];
};
struct websocket_client_data_3_header_t
{
    // 0 bit: FIN标志
    // 1-3 bit: 3位保留
    // 4-7 bit: 操作码
    unsigned char fin_rsv_oc;
    // 0 bit: 掩码
    // 1-7 bit: 数据长度
    unsigned char mask_len;
    // 如果len的是127，则后面8个字节形成的64bits无符号整型数的值是payload的真实长度。注意，网络字节序，需要转换。
    unsigned char extend_len[8];
    // 掩码密钥（当mask为1时有效）
    unsigned char mask_key[4];
};
struct websocket_server_data_1_header_t
{
    // 0 bit: FIN标志
    // 1-3 bit: 3位保留
    // 4-7 bit: 操作码
    unsigned char fin_rsv_oc;
    // 0 bit: 掩码
    // 1-7 bit: 数据长度
    // 如果len的值在0-125，则是payload的真实长度。
    unsigned char mask_len;
};
struct websocket_server_data_2_header_t
{
    // 0 bit: FIN标志
    // 1-3 bit: 3位保留
    // 4-7 bit: 操作码
    unsigned char fin_rsv_oc;
    // 0 bit: 掩码
    // 1-7 bit: 数据长度
    unsigned char mask_len;
    // 如果len的是126，则后面2个字节形成的16bits无符号整型数的值是payload的真实长度。注意，网络字节序，需要转换。
    unsigned char extend_len[2];
};
struct websocket_server_data_3_header_t
{
    // 0 bit: FIN标志
    // 1-3 bit: 3位保留
    // 4-7 bit: 操作码
    unsigned char fin_rsv_oc;
    // 0 bit: 掩码
    // 1-7 bit: 数据长度
    unsigned char mask_len;
    // 如果len的是127，则后面8个字节形成的64bits无符号整型数的值是payload的真实长度。注意，网络字节序，需要转换。
    unsigned char extend_len[8];
};
#pragma pack()
class http_server;
enum class websocket_data_type
{
    TextData,   // 文本数据
    BinaryData, // 二进制数据
    Open,
    Close,
};
struct websocket_param_t
{
    SOCKET                                       sock;     // socket
    std::string                                  url;      // url
    std::unordered_map<std::string, std::string> param;    // param
    std::string                                  frame;    // frame
    websocket_data_type                          type;     // 数据类型
    std::string                                  text_msg; // 文本（当type为websocket_data_type::Text时有效）
    byte_array
                                 binary_data; // 二进制（当type为websocket_data_type::BinaryData时有效）
    std::shared_ptr<http_server> p_server;    // server指针
};
enum websocket_pkg_type : uint8_t
{
    WEBSOCKET_OP_MIDDLE_PKG     = 0x0,
    WEBSOCKET_OP_TEXT_PKG       = 0x1,
    WEBSOCKET_OP_BINARY_PKG     = 0x2,
    WEBSOCKET_OP_DISCONNECT_PKG = 0x08,
    WEBSOCKET_OP_PING_PKG       = 0x09,
    WEBSOCKET_OP_PONG_PKG       = 0x0A
};
template <typename T>
byte_array make_server_websocket_data_pkg(const T& data);
template <typename T>
bool is_fin(const T& header);
template <typename T>
bool               with_mask(const T& header);
unsigned long long get_size(
    const websocket_client_data_1_header_t& header);
void               decode_websocket_pkg(byte_array& data, const unsigned char* mask_key);
unsigned long long get_size(
    const websocket_client_data_2_header_t& header);
template <typename T>
int                get_op_code(const T& header);
unsigned long long get_size(
    const websocket_client_data_3_header_t& header);

template <typename T>
int get_op_code(const T& header)
{
    return header.fin_rsv_oc & 0b00001111;
}

template <typename T>
byte_array make_server_websocket_data_pkg(const T& data)
{
    byte_array ret;
    int        type;
    if constexpr (std::is_same_v<T, std::string>)
    {
        type = WEBSOCKET_OP_TEXT_PKG;
    }
    else
    {
        type = WEBSOCKET_OP_BINARY_PKG;
    }
    if (data.size() < 126)
    {
        websocket_server_data_1_header_t header {};
        memset(&header, 0, sizeof(header));
        header.fin_rsv_oc = 0b10000000;
        header.fin_rsv_oc |= type;
        header.mask_len = static_cast<unsigned char>(static_cast<char>(data.size()));
        ret += to_byte_array(header);
        std::bitset<8> b1(header.fin_rsv_oc);
        std::bitset<8> b2(header.mask_len);
    }
    else if (data.size() >= 126 && data.size() <= 0xffffffffffffffffUL)
    {
        websocket_server_data_2_header_t header {};
        memset(&header, 0, sizeof(header));
        header.mask_len   = 126;
        header.fin_rsv_oc = 0b10000000;
        header.fin_rsv_oc |= type;
        *reinterpret_cast<unsigned short*>(header.extend_len) = htons(static_cast<unsigned short>(data.size()));
        ret += to_byte_array(header);
    }
    else
    {
        websocket_server_data_2_header_t header {};
        memset(&header, 0, sizeof(header));
        header.mask_len   = 127;
        header.fin_rsv_oc = 0b10000000;
        header.fin_rsv_oc |= type;
        *reinterpret_cast<unsigned short*>(header.extend_len) = static_cast<unsigned short>(
            hton64(static_cast<unsigned short>(data.size())));
        ret += to_byte_array(header);
    }
    if constexpr (std::is_same_v<T, std::string>)
    {
        ret += to_byte_array(data);
    }
    else
    {
        ret += data;
    }
    return ret;
}
template <typename T>
bool is_fin(const T& header)
{
    return static_cast<bool>(header.fin_rsv_oc & 0b10000000);
}
template <typename T>
bool with_mask(const T& header)
{
    return static_cast<bool>(header.mask_len & 0b10000000);
}
} // namespace skyfire
