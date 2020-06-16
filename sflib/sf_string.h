#pragma once
#include "sf_stdc++.h"

namespace skyfire {
template <typename StringType>
class sf_basic_string : public StringType {
public:
    using StringType::StringType;

    sf_basic_string(const StringType& str);

    static sf_basic_string<StringType> repeat(const sf_basic_string<StringType>& src, int count);

};

using sf_string = sf_basic_string<std::string>;

template<typename StringType>
sf_basic_string<StringType> operator*(const sf_basic_string<StringType>& str, int n);

}