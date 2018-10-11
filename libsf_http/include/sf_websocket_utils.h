#pragma once

#include <memory>

#include "sf_type.hpp"
#include "sf_tcputils.hpp"
#include "sf_serialize_binary.hpp"

#ifdef _WIN32
#include <winsock2.h>
#else

#include <arpa/inet.h>
#endif

namespace skyfire {

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

    struct websocket_client_data_3_header_t {
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

    struct websocket_server_data_3_header_t {
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

    class sf_http_server;

    enum class websocket_data_type{
        TextData,
        BinaryData
    };

    struct websocket_param_t {
        SOCKET sock;
        std::string url;
        websocket_data_type type;
        std::string text_msg;
        byte_array binary_data;
        std::shared_ptr<sf_http_server> p_server;
    };

    enum websocket_pkg_type
    {
        WEBSOCKET_OP_MIDDLE_PKG = 0x0,
        WEBSOCKET_OP_TEXT_PKG = 0x1,
        WEBSOCKET_OP_BINARY_PKG = 0x2,
        WEBSOCKET_OP_DISCONNECT_PKG = 0x08,
        WEBSOCKET_OP_PING_PKG = 0x09,
        WEBSOCKET_OP_PONG_PKG = 0x0A
    };

    template<typename T>
    byte_array make_server_websocket_data_pkg(const T &data) ;

    template<typename T>
    bool sf_is_fin(const T &header) ;

    template<typename T>
    bool sf_with_mask(const T &header) ;

    unsigned long long sf_get_size(const websocket_client_data_1_header_t &header) ;

    void sf_decode_websocket_pkg(byte_array &data, const unsigned char *mask_key) ;

    unsigned long long sf_get_size(const websocket_client_data_2_header_t &header) ;


    template <typename T>
    bool sf_is_fin(const T& header);
    template <typename T>
    inline bool sf_with_mask(const T& header);
    template <typename T>
    inline int sf_get_op_code(const T&header);

    inline unsigned long long sf_get_size(const websocket_client_data_1_header_t &header);
    inline void sf_decode_websocket_pkg(byte_array &data, const unsigned char* mask_key);

}






