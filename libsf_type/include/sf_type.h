/*
 * 常用类型声明
 */

#pragma once

#include <vector>
#include <string>
#include <fstream>

#ifndef _WIN32
#define SOCKET int
#endif

#define SF_GET_OFFSET(obj,member)   (reinterpret_cast<size_t>(&(reinterpret_cast<obj*>(0)->member)))

#include <iostream>
#include <cstring>


namespace skyfire
{
    using byte_array = std::vector<char>;

    inline std::string to_string(const byte_array& data);

    inline byte_array operator+(byte_array b1, const byte_array &b2);

    inline byte_array &operator+=(byte_array &b1, const byte_array &b2);

    template <typename T>
    inline byte_array to_byte_array(const T& t);

    inline byte_array to_byte_array(const std::string& str);

    inline bool dump_byte_array(const byte_array& data, const std::string &filename,bool app = false);
}
