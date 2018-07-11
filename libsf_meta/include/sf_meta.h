/*
 * sf_meta 元编程辅助函数
 */

#pragma once

#include <functional>
#include <tuple>

namespace skyfire {

    template<class T>
    struct sf_check_param_reference {
    };

    template<class T, class ... U>
    struct sf_check_param_reference<std::function<T(U...)>> {
        static constexpr bool value = (std::is_reference<U>::value || ... || false);
    };

    template<template<typename> class T, typename U, typename...V>
    struct sf_check_param_reference<T<U(V...)>> {
        static constexpr bool value = (std::is_reference<V>::value || ... || false);
    };

    template<class T>
    struct sf_check_param_pointer {
    };

    template<class T, class ... U>
    struct sf_check_param_pointer<std::function<T(U...)>> {
        static constexpr bool value = (std::is_pointer<U>::value || ... || false);
    };

    template<template<typename> class T, typename U, typename...V>
    struct sf_check_param_pointer<T<U(V...)>> {
        static constexpr bool value = (std::is_pointer<V>::value || ... || false);
    };


    template<class T>
    struct sf_check_return_reference {
    };

    template<class T, class ... U>
    struct sf_check_return_reference<std::function<T(U...)>> {
        static constexpr bool value = std::is_reference<T>::value;
    };

    template<template<typename> class T, typename U, typename...V>
    struct sf_check_return_reference<T<U(V...)>> {
        static constexpr bool value = std::is_reference<U>::value;
    };

    template<class T>
    struct sf_check_return_pointer {
    };

    template<class T, class ... U>
    struct sf_check_return_pointer<std::function<T(U...)>> {
        static constexpr bool value = std::is_pointer<T>::value;
    };

    template<template<typename> class T, typename U, typename...V>
    struct sf_check_return_pointer<T<U(V...)>> {
        static constexpr bool value = std::is_pointer<U>::value;
    };


    template<class T>
    struct sf_function_type_helper {
    };

    template<class T, class ... U>
    struct sf_function_type_helper<std::function<T(U...)>> {
        using return_type=T;
        using param_type= std::tuple<U...>;
    };


    template<template<typename> class T, typename U, typename...V>
    struct sf_function_type_helper<T<U(V...)>> {
        using return_type = U;
        using param_type = std::tuple<V...>;
    };

    template<typename Function, typename Tuple, std::size_t... Index>
    decltype(auto) sf_invoke_impl(Function &&func, Tuple &&t, std::index_sequence<Index...>);

    template<typename Function, typename Tuple>
    decltype(auto) sf_invoke(Function &&func, Tuple &&t);


    template<typename _Type, typename Tuple, std::size_t... Index>
    _Type *sf_make_obj_from_tuple_impl(Tuple &&t, std::index_sequence<Index...>);


    template<typename _Type, typename Tuple>
    _Type *sf_make_obj_from_tuple(Tuple &&t);


    //////////////////////////////////////////////////////////////////////




    template<typename Function, typename Tuple, std::size_t... Index>
    decltype(auto) sf_invoke_impl(Function &&func, Tuple &&t, std::index_sequence<Index...>) {
        if constexpr (std::is_same<decltype(func(std::get<Index>(std::forward<Tuple>(t))...)), void>::value)
                {
                        func(std::get<Index>(std::forward<Tuple>(t))...);
                return 0;
                } else
        {
            return func(std::get<Index>(std::forward<Tuple>(t))...);
        }
    }

    template<typename Function, typename Tuple>
    decltype(auto) sf_invoke(Function &&func, Tuple &&t) {
        constexpr auto size = std::tuple_size<typename std::decay<Tuple>::type>::value;
        return sf_invoke_impl(std::forward<Function>(func), std::forward<Tuple>(t), std::make_index_sequence<size>{});
    }


    template<typename _Type, typename Tuple, std::size_t... Index>
    _Type *sf_make_obj_from_tuple_impl(Tuple &&t, std::index_sequence<Index...>) {
        return new _Type(std::get<Index>(std::forward<Tuple>(t))...);
    }

    template<typename _Type, typename Tuple>
    _Type *sf_make_obj_from_tuple(Tuple &&t) {
        constexpr auto size = std::tuple_size<typename std::decay<Tuple>::type>::value;
        return sf_make_obj_from_tuple_impl<_Type>(std::forward<Tuple>(t), std::make_index_sequence<size>{});
    }



    /////////////////////////////////////////////////////////
    // make_placeholders

    template<template<int> typename _Placeholders>
    struct __placeholders_type {
        template<int N>
        __placeholders_type(_Placeholders<N>) {};
        template<int N>
        using type = _Placeholders<N>;
    };

    template<template<int> typename _Placeholders, int N>
    __placeholders_type(_Placeholders<N>)
    ->
    __placeholders_type<_Placeholders>;

    template<int N>
    auto make_placeholders() {
        return typename
        decltype(__placeholders_type(std::placeholders::_1))::template type<N>{};
    }
}
