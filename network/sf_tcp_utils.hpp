
/**
* @version 1.0.0
* @author skyfire
* @mail skyfireitdiy@hotmail.com
* @see http://github.com/skyfireitdiy/sflib
* @file sf_tcputils.hpp

* sflib第一版本发布
* 版本号1.0.0
* 发布日期：2018-10-22
*/

#pragma once

#include "sf_tcp_utils.h"

namespace skyfire {

template <typename T>
typename std::enable_if<std::is_pod<T>::value, byte_array>::type make_pkg(
    const T &data) {
    byte_array ret(sizeof(data));
#ifdef _WIN32
    memmove_s(ret.data(), sizeof(data), &data, sizeof(data));
#else
    memmove(ret.data(), &data, sizeof(data));
#endif
    return ret;
}

#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCUnusedGlobalDeclarationInspection"
template <typename T>
typename std::enable_if<std::is_pod<T>::value, T>::type take_pkg(
    const byte_array &data) {
    assert(data.size() == sizeof(T));
    T ret;
#ifdef _WIN32
    memmove_s(&ret, sizeof(ret), data.data(), sizeof(ret));
#else
    memmove(&ret, data.data(), sizeof(ret));
#endif
    return ret;
}
#pragma clang diagnostic pop

inline void make_header_checksum(sf_pkg_header_t &header) {
    header.checksum = 0;
    auto offset = SF_GET_OFFSET(sf_pkg_header_t, type);
    auto p_byte = reinterpret_cast<const unsigned char *>(&header);
    for (auto i = offset; i < sizeof(header); ++i) {
        header.checksum ^= p_byte[i];
    }
}

inline bool check_header_checksum(const sf_pkg_header_t &header) {
    unsigned char checksum = 0;
    auto offset = SF_GET_OFFSET(sf_pkg_header_t, type);
    auto p_byte = reinterpret_cast<const unsigned char *>(&header);
    for (auto i = offset; i < sizeof(header); ++i) {
        checksum ^= p_byte[i];
    }
    return checksum == header.checksum;
}

inline unsigned long long sf_ntoh64(unsigned long long input) {
    unsigned long long val;
    auto *data = reinterpret_cast<unsigned char *>(&val);

    data[0] = static_cast<unsigned char>(input >> 56);
    data[1] = static_cast<unsigned char>(input >> 48);
    data[2] = static_cast<unsigned char>(input >> 40);
    data[3] = static_cast<unsigned char>(input >> 32);
    data[4] = static_cast<unsigned char>(input >> 24);
    data[5] = static_cast<unsigned char>(input >> 16);
    data[6] = static_cast<unsigned char>(input >> 8);
    data[7] = static_cast<unsigned char>(input >> 0);
    return val;
}

inline unsigned long long sf_hton64(unsigned long long input) {
    return (sf_ntoh64(input));
}
}    // namespace skyfire
