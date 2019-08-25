
/**
* @version 1.0.0
* @author skyfire
* @mail skyfireitdiy@hotmail.com
* @see http://github.com/skyfireitdiy/sflib
* @file sf_meta.hpp

* sflib第一版本发布
* 版本号1.0.0
* 发布日期：2018-10-22
*/

/*
 * sf_meta 元编程辅助函数
 */
#pragma once
#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCUnusedGlobalDeclarationInspection"

#include "sf_meta.h"

namespace skyfire {

template <int N>
auto make_placeholders() {
    return typename decltype(
        __sf_placeholders_type(std::placeholders::_1))::template type<N>{};
}

template <typename _Type, typename Tuple>
_Type *sf_make_obj_from_tuple(Tuple &&t) {
    constexpr auto size =
        std::tuple_size<typename std::decay<Tuple>::type>::value;
    return sf_make_obj_from_tuple_impl<_Type>(std::forward<Tuple>(t),
                                              std::make_index_sequence<size>{});
}

template <typename _Type, typename Tuple, size_t... Index>
_Type *sf_make_obj_from_tuple_impl(Tuple &&t, std::index_sequence<Index...>) {
    return new _Type(std::get<Index>(std::forward<Tuple>(t))...);
}

template <typename Function, typename Tuple>
decltype(auto) sf_invoke(Function &&func, Tuple &&t) {
    constexpr auto size =
        std::tuple_size<typename std::decay<Tuple>::type>::value;
    return sf_invoke_impl(std::forward<Function>(func), std::forward<Tuple>(t),
                          std::make_index_sequence<size>{});
}

template <typename Function, typename Tuple, size_t... Index>
decltype(auto) sf_invoke_impl(Function &&func, Tuple &&t,
                              std::index_sequence<Index...>) {
    if constexpr (std::is_same<decltype(func(
                                   std::get<Index>(std::forward<Tuple>(t))...)),
                               void>::value) {
        func(std::get<Index>(std::forward<Tuple>(t))...);
        return 0;
    } else {
        return func(std::get<Index>(std::forward<Tuple>(t))...);
    }
}

}    // namespace skyfire

#pragma clang diagnostic pop