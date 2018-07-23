#pragma once

#include "sf_type.h"

#pragma pack(1)

struct websocket_data_header_t
{
    // 0 bit: FIN标志
    // 1-3 bit: 3位保留
    // 4-7 bit: 操作码
    unsigned char fin_rsv_oc;
    // 0 bit: 掩码
    // 1-7 bit: 数据长度
    unsigned char mask_len;
    // 如果len的值在0-125，则是payload的真实长度。
    // 如果len的是126，则后面2个字节形成的16bits无符号整型数的值是payload的真实长度。注意，网络字节序，需要转换。
    // 如果len的是127，则后面8个字节形成的64bits无符号整型数的值是payload的真实长度。注意，网络字节序，需要转换。
    unsigned char extend_len[8];
    // 掩码密钥（当mask为1时有效）
    unsigned int mask_key;
};

struct websocket_data_frame_t
{
    websocket_data_header_t header;
    byte_array body;
};


#pragma pack()