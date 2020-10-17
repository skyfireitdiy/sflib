#pragma once

#include "sf_stdc++.h"

namespace skyfire {
template <typename... Type>
class multi_value {
private:
    std::tuple<std::decay_t<Type>...> data__;

public:
    multi_value() = default;

    multi_value(std::decay_t<Type>... value)
        : data__(value...)
    {
    }

    template <typename T>
    operator T() const {
        return std::get<std::decay_t<T>>(data__);
    }
};

template <typename... Type>
multi_value(Type... value) -> multi_value<Type...>;

}