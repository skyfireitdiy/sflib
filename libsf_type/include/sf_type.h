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

#include <iostream>

using namespace std;

namespace skyfire
{
    using byte_array = std::vector<char>;

    inline std::string to_string(const byte_array& data)
    {
        auto ret = std::string(data.begin(),data.end());
        ret.push_back(0);
        return ret;
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

    template <typename T>
    inline byte_array to_byte_array(const T& t)
    {
        byte_array ret(sizeof(t));
        memcpy(ret.data(),&t,sizeof(t));
        return ret;
    }

    template <>
    inline byte_array to_byte_array(const std::string& str)
    {

        cout<<str.length()<<endl;
        cout<<str.length()<<endl;
        byte_array ret(str.length());
        memcpy(ret.data(),str.data(),str.length());
        return ret;
    }
}
