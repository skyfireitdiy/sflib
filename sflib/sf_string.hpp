#pragma once
#include "sf_string.h"

namespace skyfire {

template <typename StringType>
sf_basic_string<StringType>::sf_basic_string(const StringType& str)
    : StringType(str)
{
}

template <typename StringType>
sf_basic_string<StringType> sf_basic_string<StringType>::repeat(const sf_basic_string<StringType>& src, int count)
{
    auto len = src.length();
    sf_basic_string<StringType> ret(len * count, '\0');

    for (int i = 0; i < count; ++i) {
        memcpy(ret.data() + i * len, src.data(), len);
    }
    return ret;
}

template <typename StringType>
sf_basic_string<StringType> operator*(const sf_basic_string<StringType>& str, int n){
    return sf_basic_string<StringType>::repeat(str, n);
}
}