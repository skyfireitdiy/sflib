#pragma once
#include "sf_stdc++.h"

namespace skyfire
{
template <typename StringType>
class basic_string : public StringType
{
public:
    using StringType::StringType;

    basic_string(const StringType& str);

    static basic_string<StringType> repeat(const basic_string<StringType>& src, int count);
};

using sf_string = basic_string<std::string>;

template <typename StringType>
basic_string<StringType> operator*(const basic_string<StringType>& str, int n);

}
