#pragma once

#ifdef _WIN32
#include <winsock2.h>
#else
#include <unistd.h>
#include <sys/types.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#endif

#include <vector>
#include <cassert>
#include <cstring>
#include <string>
#include "sf_type.hpp"
#include "sf_define.hpp"

namespace skyfire
{
#pragma pack(1)

    struct pkg_header_t
    {
        unsigned char checksum;
        int type;
        size_t length;
    };

#pragma pack()

    template<typename T>
    typename std::enable_if<std::is_pod<T>::value, byte_array>::type make_pkg(const T &data);

    template<typename T>
    typename std::enable_if<std::is_pod<T>::value, T>::type take_pkg(const byte_array &data);

    inline void make_header_checksum(pkg_header_t& header);

    inline bool check_header_checksum(const pkg_header_t& header);


    inline unsigned long long sf_ntoh64(unsigned long long input);

    inline unsigned long long sf_hton64(unsigned long long input);

}
