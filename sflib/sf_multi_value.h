#pragma once

#include "sf_stdc++.h"

namespace skyfire {
template <typename... Type>
class sf_multi_value {
private:
    std::tuple<std::decay_t<Type>...> data__;

public:
    sf_multi_value() = default;

    sf_multi_value(std::decay_t<Type>... value)
        : data__(value...)
    {
    }

    template <typename T>
    operator T() const {
        return std::get<std::decay_t<T>>(data__);
    }
};

template <typename... Type>
sf_multi_value(Type... value) -> sf_multi_value<Type...>;

}