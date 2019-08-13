
/**
* @version 1.0.0
* @author skyfire
* @mail skyfireitdiy@hotmail.com
* @see http://github.com/skyfireitdiy/sflib
* @file sf_websocket_utils.hpp

* sflib第一版本发布
* 版本号1.0.0
* 发布日期：2018-10-22
*/

#pragma once

#include "sf_websocket_utils.h"

namespace skyfire {

template <typename T>
byte_array make_server_websocket_data_pkg(const T &data) {
    byte_array ret;
    int type;
    if constexpr (std::is_same_v<T, std::string>) {
        type = WEBSOCKET_OP_TEXT_PKG;
    } else {
        type = WEBSOCKET_OP_BINARY_PKG;
    }
    if (data.size() < 126) {
        sf_websocket_server_data_1_header_t header{};
        memset(&header, 0, sizeof(header));
        header.fin_rsv_oc = 0b10000000;
        header.fin_rsv_oc |= type;
        header.mask_len =
            static_cast<unsigned char>(static_cast<char>(data.size()));
        ret += to_byte_array(header);

        std::bitset<8> b1(header.fin_rsv_oc);
        std::bitset<8> b2(header.mask_len);
        sf_debug(b1.to_string(), b2.to_string());

    } else if (data.size() >= 126 && data.size() <= 0xffffffffffffffffUL) {
        sf_websocket_server_data_2_header_t header{};
        memset(&header, 0, sizeof(header));
        header.mask_len = 126;
        header.fin_rsv_oc = 0b10000000;
        header.fin_rsv_oc |= type;
        *reinterpret_cast<unsigned short *>(header.extend_len) =
            htons(static_cast<unsigned short>(data.size()));
        ret += to_byte_array(header);
    } else {
        sf_websocket_server_data_2_header_t header{};
        memset(&header, 0, sizeof(header));
        header.mask_len = 127;
        header.fin_rsv_oc = 0b10000000;
        header.fin_rsv_oc |= type;
        *reinterpret_cast<unsigned short *>(header.extend_len) =
            static_cast<unsigned short>(
                sf_hton64(static_cast<unsigned short>(data.size())));
        ret += to_byte_array(header);
    }
    if constexpr (std::is_same_v<T, std::string>) {
        ret += to_byte_array(data);
    } else {
        ret += data;
    }
    return ret;
}

template <typename T>
bool sf_is_fin(const T &header) {
    return static_cast<bool>(header.fin_rsv_oc & 0b10000000);
}

template <typename T>
bool sf_with_mask(const T &header) {
    return static_cast<bool>(header.mask_len & 0b10000000);
}

inline unsigned long long sf_get_size(
    const sf_websocket_client_data_1_header_t &header) {
    return static_cast<unsigned long long int>(header.mask_len & 0b01111111);
    ;
}

inline void sf_decode_websocket_pkg(byte_array &data,
                                    const unsigned char *mask_key) {
    auto size = data.size();
    for (size_t i = 0; i < size; ++i) {
        data[i] ^= mask_key[i % 4];
    }
}

inline unsigned long long sf_get_size(
    const sf_websocket_client_data_2_header_t &header) {
    return ntohs(*reinterpret_cast<const unsigned short *>(header.extend_len));
}

template <typename T>
inline int sf_get_op_code(const T &header) {
    return header.fin_rsv_oc & 0b00001111;
}

inline unsigned long long sf_get_size(
    const sf_websocket_client_data_3_header_t &header) {
    return sf_ntoh64(
        *reinterpret_cast<const unsigned long long *>(header.extend_len));
}

}    // namespace skyfire
