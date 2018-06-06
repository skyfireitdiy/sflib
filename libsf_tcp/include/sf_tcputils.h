#pragma once

#include <vector>
#include <cassert>
#include <cstring>
#include <string>
#include "sf_type.h"
#include "sf_define.h"

namespace skyfire
{



    struct alignas(4) pkg_header_t
    {
        unsigned char checksum;
        int type;
        size_t length;
    };

    template<typename T>
    typename std::enable_if<std::is_pod<T>::value, byte_array>::type make_pkg(const T &data)
    {
        byte_array ret(sizeof(data));
#ifdef _WIN32
        memmove_s(ret.data(), sizeof(data), &data, sizeof(data));
#else
        memmove(ret.data(), &data, sizeof(data));
#endif
        return ret;
    }

    template<typename T>
    typename std::enable_if<std::is_pod<T>::value, T>::type take_pkg(const byte_array &data)
    {
        assert(data.size() == sizeof(T));
        T ret;
#ifdef _WIN32
        memmove_s(&ret, sizeof(ret), data.data(), sizeof(ret));
#else
        memmove(&ret, data.data(), sizeof(ret));
#endif
        return ret;
    }

    inline void make_header_checksum(pkg_header_t& header)
    {
        header.checksum = 0;
        auto offset = SF_GET_OFFSET(pkg_header_t, type);
        auto p_byte = reinterpret_cast<const unsigned char*>(&header);
        for(auto i = offset; i < sizeof(header); ++i)
        {
            header.checksum ^= p_byte[i];
        }
    }

    inline bool check_header_checksum(const pkg_header_t& header)
    {
        unsigned char checksum = 0;
        auto offset = SF_GET_OFFSET(pkg_header_t, type);
        auto p_byte = reinterpret_cast<const unsigned char*>(&header);
        for(auto i = offset; i < sizeof(header); ++i)
        {
            checksum ^= p_byte[i];
        }
        return checksum == header.checksum;
    }
}
