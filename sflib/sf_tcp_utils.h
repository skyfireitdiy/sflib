
/**
* @version 1.0.0
* @author skyfire
* @file sf_tcp_utils.h
*/

#pragma once

#include "sf_define.h"
#include "sf_option.h"
#include "sf_type.hpp"
#include <arpa/inet.h>
#include <cassert>
#include <cstring>
#include <errno.h>
#include <netinet/in.h>
#include <string>
#include <sys/socket.h>
#include <sys/types.h>
#include <thread>
#include <unistd.h>
#include <vector>

namespace skyfire
{
#pragma pack(1)

/**
 *  @brief 包头
 */
struct pkg_header_t
{
    unsigned char checksum; // 校验和
    size_t        type;     // 包类型
    size_t        length;   // 包长度
};

#pragma pack()

struct tcp_server_opt_t
{
    bool   raw            = false;
    bool   manage_read    = true;
    size_t thread_count   = std::thread::hardware_concurrency() * 2 + 2;
    bool   manage_clients = true;
};

using tcp_server_opt_option = option<tcp_server_opt_t>;
using tcp_server_config     = tcp_server_opt_option::OptionFuncType;

namespace tcp
{
    namespace
    {
        auto raw = tcp_server_opt_option::make_option(std::function([](tcp_server_opt_t& opt, bool r) {
            opt.raw = r;
        }));

        auto thread_count = tcp_server_opt_option::make_option(std::function([](tcp_server_opt_t& opt, int count) {
            if (count < 1)
            {
                count = 1;
            }
            opt.thread_count = count;
        }));

        auto manage_read = tcp_server_opt_option::make_option(std::function([](tcp_server_opt_t& opt, bool read) {
            opt.manage_read = read;
        }));

        auto manage_clients = tcp_server_opt_option::make_option(std::function([](tcp_server_opt_t& opt, bool mc) {
            opt.manage_clients = mc;
        }));
    }
}

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

/**
 * 解析数据包
 * @tparam T 数据类型
 * @param data 数据
 * @return 解析出来的数据包
 */
template <typename T>
typename std::enable_if<std::is_standard_layout<T>::value && std::is_trivial<T>::value, T>::type take_pkg(
    const byte_array& data);

/**
 * 计算header的校验和
 * @param header 包头
 */
inline void make_header_checksum(pkg_header_t& header);

/**
 * 校验header的校验和
 * @param header 包头
 * @return 是否通过校验
 */
inline bool check_header_checksum(const pkg_header_t& header);

/**
 * 64位整型网络字节序转主机字节序
 * @param input 整型数字（网络字节序）
 * @return 整型数字（主机字节序）
 */
inline unsigned long long ntoh64(unsigned long long input);

/**
 * 64位整型主机字节序转网络字节序
 * @param input 整型数字（主机字节序）
 * @return 整型数字（网络字节序）
 */
inline unsigned long long hton64(unsigned long long input);

/**
 * @brief 是否是大端序
 * 
 * @return true 大端序
 * @return false 非大端序
 */
inline bool big_endian();

} // namespace skyfire
