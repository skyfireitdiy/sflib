#pragma once

#include <vector>
#include <cassert>
#include <string>
#include "sf_type.h"

namespace skyfire
{

    constexpr size_t BUFFER_SIZE = 4096;

    struct alignas(1) pkg_header_t
    {
        int type;
        size_t length;
    };

    inline std::string ba2str(const byte_array &data)
    {
        return std::string(data.begin(), data.end());
    }

    inline byte_array str2ba(const std::string &str)
    {
        return byte_array(str.begin(), str.end());
    }

    template<typename T>
    typename std::enable_if<std::is_pod<T>::value, byte_array>::type make_pkg(const T &data)
    {
        byte_array ret(sizeof(data));
        memmove_s(ret.data(), sizeof(data), &data, sizeof(data));
        return ret;
    }

    template<typename T>
    typename std::enable_if<std::is_pod<T>::value, T>::type take_pkg(const byte_array &data)
    {
        assert(data.size() == sizeof(T));
        T ret;
        memmove_s(&ret, sizeof(ret), data.data(), sizeof(ret));
        return ret;
    }

}
