#pragma once

#include "sf_stdc++.h"

namespace skyfire {
template <typename... Type>
class sf_multi_value {
private:
    std::tuple<Type...> data__;

public:
    sf_multi_value() = default;

    sf_multi_value(Type... value)
        : data__(value...)
    {
    }

    template <typename T>
    operator T() const {
        return std::get<T>(data__);
    }
};
}