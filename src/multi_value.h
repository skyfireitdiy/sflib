#pragma once
#include "stdc++.h"
namespace skyfire
{
template <typename... Type>
class multi_value : public std::tuple<Type...>
{
private:
public:
    using std::tuple<Type...>::tuple;
    template <typename T>
    operator T() const
    {
        return std::get<std::decay_t<T>>(*this);
    }
};
template <typename... Type>
multi_value(Type... value) -> multi_value<Type...>;
}