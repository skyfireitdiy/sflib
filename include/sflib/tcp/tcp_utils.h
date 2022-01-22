
#pragma once
#include "sflib/core/type.h"
#include "sflib/tools/define.h"
#include "sflib/tools/option.h"
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
        auto raw            = tcp_server_opt_option::make_option(std::function([](tcp_server_opt_t& opt, bool r) {
            opt.raw = r;
        }));
        auto thread_count   = tcp_server_opt_option::make_option(std::function([](tcp_server_opt_t& opt, int count) {
            if (count < 1)
            {
                count = 1;
            }
            opt.thread_count = count;
        }));
        auto manage_read    = tcp_server_opt_option::make_option(std::function([](tcp_server_opt_t& opt, bool read) {
            opt.manage_read = read;
        }));
        auto manage_clients = tcp_server_opt_option::make_option(std::function([](tcp_server_opt_t& opt, bool mc) {
            opt.manage_clients = mc;
        }));
    }
}
constexpr int max_tcp_connection = 1000000;
template <typename T>
typename std::enable_if<std::is_standard_layout<T>::value && std::is_trivial<T>::value, byte_array>::type make_pkg(
    const T& data);
template <typename T>
typename std::enable_if<std::is_standard_layout<T>::value && std::is_trivial<T>::value, T>::type take_pkg(
    const byte_array& data);
void               make_header_checksum(pkg_header_t& header);
bool               check_header_checksum(const pkg_header_t& header);
unsigned long long ntoh64(unsigned long long input);
unsigned long long hton64(unsigned long long input);
bool               big_endian();

template <typename T>
typename std::enable_if<std::is_standard_layout<T>::value && std::is_trivial<T>::value, byte_array>::type make_pkg(
    const T& data)
{
    byte_array ret(sizeof(data));
    memmove(ret.data(), &data, sizeof(data));
    return ret;
}
template <typename T>
typename std::enable_if<std::is_standard_layout<T>::value && std::is_trivial<T>::value, T>::type take_pkg(
    const byte_array& data)
{
    assert(data.size() == sizeof(T));
    T ret;
    memmove(&ret, data.data(), sizeof(ret));
    return ret;
}
} // namespace skyfire
