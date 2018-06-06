/*
 * 常用类型声明
 */

#pragma once

#include <vector>
#include <string>

#ifndef _WIN32
#define SOCKET int
#endif

#define SF_GET_OFFSET(obj,member)   (reinterpret_cast<size_t>(&(reinterpret_cast<obj*>(0)->member)))

namespace skyfire
{
    using byte_array = std::vector<char>;

    inline std::string to_string(const byte_array& data)
    {
        return std::string(data.begin(),data.end());
    }

    inline byte_array operator+(byte_array b1, const byte_array &b2)
    {
        b1.insert(b1.end(), b2.begin(), b2.end());
        return b1;
    }

    inline byte_array &operator+=(byte_array &b1, const byte_array &b2)
    {
        b1.insert(b1.end(), b2.begin(), b2.end());
        return b1;
    }


}
