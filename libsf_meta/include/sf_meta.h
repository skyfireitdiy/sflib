#pragma once

#include <functional>
#include <tuple>

namespace skyfire
{

    template<class T>
    struct sf_function_type_helper
    {
    };

    template<class T, class ... U>
    struct sf_function_type_helper<std::function<T(U...)>>
    {
        using return_type=T;
        using param_type= std::tuple<U...>;
    };

    template<typename T>
    struct sf_bind_type_helper
    {
    };

    template<template<typename> class T, typename U, typename...V>
    struct sf_bind_type_helper<T<U(V...)>>
    {
        using return_type = U;
        using param_type = std::tuple<V...>;
    };

    template<typename Function, typename Tuple, std::size_t... Index>
    decltype(auto) sf_invoke_impl(Function&& func, Tuple&& t, std::index_sequence<Index...>)
    {
        return func(std::get<Index>(std::forward<Tuple>(t))...);
    }

    template<typename Function, typename Tuple>
    decltype(auto) sf_invoke(Function&& func, Tuple&& t)
    {
        constexpr auto size = std::tuple_size<typename std::decay<Tuple>::type>::value;
        return sf_invoke_impl(std::forward<Function>(func), std::forward<Tuple>(t), std::make_index_sequence<size>{});
    }


    template<typename _Type, typename Tuple, std::size_t... Index>
    _Type* sf_make_obj_from_tuple_impl(Tuple&& t, std::index_sequence<Index...>)
    {
        return new _Type(std::get<Index>(std::forward<Tuple>(t))...);
    }

    template<typename _Type, typename Tuple>
    _Type* sf_make_obj_from_tuple(Tuple&& t)
    {
        constexpr auto size = std::tuple_size<typename std::decay<Tuple>::type>::value;
        return sf_make_obj_from_tuple_impl<_Type>(std::forward<Tuple>(t), std::make_index_sequence<size>{});
    }


}