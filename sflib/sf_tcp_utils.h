
/**
* @version 1.0.0
* @author skyfire
* @file sf_tcp_utils.h
*/

#pragma once

#ifdef _WIN32
#include <winsock2.h>
#else
#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#endif

#include "sf_define.h"
#include "sf_type.hpp"
#include <cassert>
#include <cstring>
#include <string>
#include <vector>

namespace skyfire {
#pragma pack(1)

/**
 *  @brief 包头
 */
struct sf_pkg_header_t {
    unsigned char checksum; // 校验和
    int type; // 包类型
    int length; // 包长度
};

#pragma pack()

constexpr int max_tcp_connection = 1000000;

/**
 * 生成数据包
 * @tparam T 数据类型
 * @param data 数据
 * @return 二进制数据包
 */
template <typename T>
typename std::enable_if<std::is_standard_layout<T>::value && std::is_trivial<T>::value, byte_array>::type make_pkg(
    const T& data);

#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCUnusedGlobalDeclarationInspection"
/**
 * 解析数据包
 * @tparam T 数据类型
 * @param data 数据
 * @return 解析出来的数据包
 */
template <typename T>
typename std::enable_if<std::is_standard_layout<T>::value && std::is_trivial<T>::value, T>::type take_pkg(
    const byte_array& data);
#pragma clang diagnostic pop

/**
 * 计算header的校验和
 * @param header 包头
 */
inline void make_header_checksum(sf_pkg_header_t& header);

/**
 * 校验header的校验和
 * @param header 包头
 * @return 是否通过校验
 */
inline bool check_header_checksum(const sf_pkg_header_t& header);

/**
 * 64位整型网络字节序转主机字节序
 * @param input 整型数字（网络字节序）
 * @return 整型数字（主机字节序）
 */
inline unsigned long long sf_ntoh64(unsigned long long input);

/**
 * 64位整型主机字节序转网络字节序
 * @param input 整型数字（主机字节序）
 * @return 整型数字（网络字节序）
 */
inline unsigned long long sf_hton64(unsigned long long input);

/**
 * @brief 是否是大端序
 * 
 * @return true 大端序
 * @return false 非大端序
 */
inline bool sf_big_endian();

} // namespace skyfire
