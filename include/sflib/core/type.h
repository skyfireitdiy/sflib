
#pragma once
#include <fstream>
#include <string>
#include <vector>
using SOCKET = int;
#define SF_GET_OFFSET(obj, member) \
    (reinterpret_cast<size_t>(&(reinterpret_cast<obj*>(0)->member)))
#include <cstring>
#include <iostream>
namespace skyfire
{
using byte_array = std::vector<char>;
std::string to_string(const byte_array& data);
byte_array  operator+(byte_array b1, const byte_array& b2);
byte_array& operator+=(byte_array& b1, const byte_array& b2);
template <typename T>
byte_array to_byte_array(const T& t);
byte_array to_byte_array(const char* s);
byte_array to_byte_array(const std::string& str);
bool       dump_byte_array(const byte_array& data, const std::string& filename,
                           bool app = false);
template <typename T>
byte_array to_byte_array(const T& t)
{
    byte_array ret(sizeof(t));
    std::memcpy(ret.data(), &t, sizeof(t));
    return ret;
}
} // namespace skyfire
