

/**
* @version 1.0.0
* @author skyfire
* @mail skyfireitdiy@hotmail.com
* @see http://github.com/skyfireitdiy/sflib
* @file sf_meta.h

* sflib第一版本发布
* 版本号1.0.0
* 发布日期：2018-10-22
*/

/*
 * sf_meta 元编程辅助函数
 */
#pragma once
#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCUnusedStructInspection"
#pragma ide diagnostic ignored "google-explicit-constructor"
#pragma ide diagnostic ignored "readability-redundant-declaration"
#pragma ide diagnostic ignored "OCUnusedGlobalDeclarationInspection"

#include "core/sf_stdc++.h"

namespace skyfire {

template <class T>
struct sf_check_param_reference;

template <class T, class... U>
struct sf_check_param_reference<std::function<T(U...)>> final {
    static constexpr bool value =
        std::disjunction<std::is_reference<U>...>::value;
    // static constexpr bool value = (std::is_reference<U>::value || ... ||
    // false);
};

template <typename T>
struct sf_check_param_reference final {
    static constexpr bool value = sf_check_param_reference<decltype(
        std::function(std::declval<T>()))>::value;
    // static constexpr bool value = (std::is_reference<V>::value || ... ||
    // false);
};

template <class T>
struct sf_check_param_pointer;

template <class T, class... U>
struct sf_check_param_pointer<std::function<T(U...)>> final {
    static constexpr bool value =
        std::disjunction<std::is_pointer<U>...>::value;
    // static constexpr bool value = (std::is_pointer<U>::value || ... ||
    // false);
};

template <typename T>
struct sf_check_param_pointer final {
    static constexpr bool value = sf_check_param_pointer<decltype(
        std::function(std::declval<T>()))>::value;
    // static constexpr bool value = (std::is_pointer<V>::value || ... ||
    // false);
};

template <class T>
struct sf_check_return_reference final {};

template <class T, class... U>
struct sf_check_return_reference<std::function<T(U...)>> final {
    static constexpr bool value = std::is_reference<T>::value;
};

template <template <typename> class T, typename U, typename... V>
struct sf_check_return_reference<T<U(V...)>> final {
    static constexpr bool value = std::is_reference<U>::value;
};

template <class T>
struct sf_check_return_pointer final {};

template <class T, class... U>
struct sf_check_return_pointer<std::function<T(U...)>> final {
    static constexpr bool value = std::is_pointer<T>::value;
};

template <template <typename> class T, typename U, typename... V>
struct sf_check_return_pointer<T<U(V...)>> final {
    static constexpr bool value = std::is_pointer<U>::value;
};

template <typename T>
struct sf_function_type_helper;

template <class T, class... U>
struct sf_function_type_helper<std::function<T(U...)>> final {
    using return_type = typename std::decay<T>::type;
    using param_type = std::tuple<typename std::decay<U>::type...>;
};

template <typename T>
struct sf_function_type_helper final {
    using return_type = typename sf_function_type_helper<decltype(
        std::function(std::declval<T>()))>::return_type;
    using param_type = typename sf_function_type_helper<decltype(
        std::function(std::declval<T>()))>::param_type;
};

template <typename Function, typename Tuple, std::size_t... Index>
decltype(auto) sf_invoke_impl(Function &&func, Tuple &&t,
                              std::index_sequence<Index...>);

template <typename Function, typename Tuple>
decltype(auto) sf_invoke(Function &&func, Tuple &&t);

template <typename _Type, typename Tuple, std::size_t... Index>
_Type *sf_make_obj_from_tuple_impl(Tuple &&t, std::index_sequence<Index...>);

template <typename _Type, typename Tuple>
_Type *sf_make_obj_from_tuple(Tuple &&t);

//////////////////////////////////////////////////////////////////////

template <typename Function, typename Tuple, std::size_t... Index>
decltype(auto) sf_invoke_impl(Function &&func, Tuple &&t,
                              std::index_sequence<Index...>);

template <typename Function, typename Tuple>
decltype(auto) sf_invoke(Function &&func, Tuple &&t);

template <typename _Type, typename Tuple, std::size_t... Index>
_Type *sf_make_obj_from_tuple_impl(Tuple &&t, std::index_sequence<Index...>);

template <typename _Type, typename Tuple>
_Type *sf_make_obj_from_tuple(Tuple &&t);

/////////////////////////////////////////////////////////
// make_placeholders

template <template <int> typename _Placeholders>
struct __sf_placeholders_type {
    template <int N>
    __sf_placeholders_type(_Placeholders<N>){};
    template <int N>
    using type = _Placeholders<N>;
};

template <template <int> typename _Placeholders, int N>
__sf_placeholders_type(_Placeholders<N>)->__sf_placeholders_type<_Placeholders>;

template <int N>
auto make_placeholders();

template <typename T>
class sf_is_shared_ptr : public std::false_type {};

template <typename T>
class sf_is_shared_ptr<std::shared_ptr<T>> : public std::true_type {};

}    // namespace skyfire

#pragma clang diagnostic pop