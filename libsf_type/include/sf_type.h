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
}
