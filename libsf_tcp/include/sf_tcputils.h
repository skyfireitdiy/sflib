#pragma once

#include <vector>
#include <cassert>

namespace skyfire
{

constexpr size_t BUFFER_SIZE = 4096;
using bytearray_t = std::vector<char>;

class no_copy
{
public:
    no_copy() = default;
    no_copy(const no_copy&) = delete;
    no_copy(no_copy&&) = delete;
    no_copy& operator=(const no_copy&) = delete;
};

struct alignas(1) pkg_header_t
{
    int type;
    size_t length;
};

inline std::string ba2str(const bytearray_t& data)
{
    return std::string(data.begin(), data.end());
}

inline bytearray_t str2ba(const std::string& str)
{
    return bytearray_t(str.begin(), str.end());
}

template<typename T>
typename std::enable_if<std::is_pod<T>::value,bytearray_t>::type make_pkg(const T& data)
{
    bytearray_t ret(sizeof(data));
    memmove_s(ret.data(), sizeof(data), &data, sizeof(data));
    return ret;
}

template<typename T>
typename std::enable_if<std::is_pod<T>::value,T>::type take_pkg(const bytearray_t &data)
{
    assert(data.size() == sizeof(T));
    T ret;
    memmove_s(&ret, sizeof(ret), data.data(), sizeof(ret));
    return ret;
}

}

