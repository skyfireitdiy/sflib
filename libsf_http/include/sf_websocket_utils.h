#pragma once

#include "sf_type.h"
#include "sf_tcputils.h"

namespace skyfire {

#pragma pack(1)

    struct websocket_data_1_header_t
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

    struct websocket_data_2_header_t
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

    struct websocket_data_3_header_t {
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

#pragma pack()


    constexpr int WEBSOCKET_OP_MIDDLE_PKG = 0x0;

    constexpr int WEBSOCKET_OP_TEXT_PKG = 0x1;

    constexpr int WEBSOCKET_OP_BINARY_PKG = 0x2;

    constexpr int WEBSOCKET_OP_DISCONNECT_PKG = 0x08;

    constexpr int WEBSOCKET_OP_PING_PKG = 0x09;

    constexpr int WEBSOCKET_OP_PONG_PKG = 0x0A;

    template <typename T>
    bool sf_is_fin(const T& header){
        return static_cast<bool>(header.fin_rsv_oc & 0b10000000);
    }

    template <typename T>
    inline bool sf_with_mask(const T& header){
        return static_cast<bool>(header.mask_len & 0b10000000);
    }

    template <typename T>
    inline int sf_get_op_code(const T&header){
        return header.fin_rsv_oc & 0b00001111;
    }


    inline unsigned long long sf_get_size(const websocket_data_1_header_t &header)
    {
        return static_cast<unsigned long long int>(header.mask_len & 0b01111111);;
    }

    inline unsigned long long sf_get_size(const websocket_data_2_header_t &header)
    {
        return ntohs(*reinterpret_cast<const unsigned short *>(header.extend_len));
    }

    inline unsigned long long sf_get_size(const websocket_data_3_header_t &header)
    {
        return sf_ntoh64(*reinterpret_cast<const unsigned long long *>(header.extend_len));
    }



    inline void sf_decode_websocket_pkg(byte_array &data, const unsigned char* mask_key){
        auto size = data.size();
        for(size_t i = 0;i<size;++i){
            data[i] ^= mask_key[i%4];
        }
    }

}






