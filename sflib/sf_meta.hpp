
#pragma once
#include "sf_meta.h"
namespace skyfire
{
template <int N>
auto make_placeholders()
{
    return typename decltype(
        __placeholders_type(std::placeholders::_1))::template type<N> {};
}
template <typename _Type, typename Tuple>
_Type* make_obj_from_tuple(Tuple&& t)
{
    constexpr auto size = std::tuple_size<typename std::decay<Tuple>::type>::value;
    return make_obj_from_tuple_impl<_Type>(std::forward<Tuple>(t),
                                           std::make_index_sequence<size> {});
}
template <typename _Type, typename Tuple, size_t... Index>
_Type* make_obj_from_tuple_impl(Tuple&& t, std::index_sequence<Index...>)
{
    return new _Type(std::get<Index>(std::forward<Tuple>(t))...);
}
template <typename Function, typename Tuple>
decltype(auto) invoke(Function&& func, Tuple&& t)
{
    constexpr auto size = std::tuple_size<typename std::decay<Tuple>::type>::value;
    return invoke_impl(std::forward<Function>(func), std::forward<Tuple>(t),
                       std::make_index_sequence<size> {});
}
template <typename Function, typename Tuple, size_t... Index>
decltype(auto) invoke_impl(Function&& func, Tuple&& t,
                           std::index_sequence<Index...>)
{
    if constexpr (std::is_same<decltype(func(
                                   std::get<Index>(std::forward<Tuple>(t))...)),
                               void>::value)
    {
        func(std::get<Index>(std::forward<Tuple>(t))...);
        return 0;
    }
    else
    {
        return func(std::get<Index>(std::forward<Tuple>(t))...);
    }
}
} // namespace skyfire
