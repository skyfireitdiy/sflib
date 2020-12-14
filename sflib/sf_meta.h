
/**
* @version 1.0.0
* @author skyfire
* @file sf_meta.h

* 
*/

/**
* @author skyfire
* @file sf_meta.h
*/

/*
 * meta 元编程辅助函数
 */
#pragma once

#include "sf_stdc++.h"

namespace skyfire
{

/**
 * @brief 检测类型是否是引用
 * 
 * @tparam T 类型
 */
template <class T>
struct check_param_reference;

template <class T, class... U>
struct check_param_reference<std::function<T(U...)>> final
{
    static constexpr bool value = std::disjunction<std::is_reference<U>...>::value;
    // static constexpr bool value = (std::is_reference<U>::value || ... ||
    // false);
};

template <typename T>
struct check_param_reference final
{
    static constexpr bool value = check_param_reference<decltype(
        std::function(std::declval<T>()))>::value;
    // static constexpr bool value = (std::is_reference<V>::value || ... ||
    // false);
};

/**
 * @brief 检测类型是否是指针
 * 
 * @tparam T 类型
 */

template <class T>
struct check_param_pointer;

template <class T, class... U>
struct check_param_pointer<std::function<T(U...)>> final
{
    static constexpr bool value = std::disjunction<std::is_pointer<U>...>::value;
    // static constexpr bool value = (std::is_pointer<U>::value || ... ||
    // false);
};

template <typename T>
struct check_param_pointer final
{
    static constexpr bool value = check_param_pointer<decltype(
        std::function(std::declval<T>()))>::value;
    // static constexpr bool value = (std::is_pointer<V>::value || ... ||
    // false);
};

/**
 * @brief 检测返回值是否是引用类型
 * 
 * @tparam T 类型
 */

template <class T>
struct check_return_reference final
{
};

template <class T, class... U>
struct check_return_reference<std::function<T(U...)>> final
{
    static constexpr bool value = std::is_reference<T>::value;
};

template <template <typename> class T, typename U, typename... V>
struct check_return_reference<T<U(V...)>> final
{
    static constexpr bool value = std::is_reference<U>::value;
};

/**
 * @brief 检测返回类型是否是指针
 * 
 * @tparam T 类型
 */
template <class T>
struct check_return_pointer final
{
};

template <class T, class... U>
struct check_return_pointer<std::function<T(U...)>> final
{
    static constexpr bool value = std::is_pointer<T>::value;
};

template <template <typename> class T, typename U, typename... V>
struct check_return_pointer<T<U(V...)>> final
{
    static constexpr bool value = std::is_pointer<U>::value;
};

/**
 * @brief 提取函数的返回类型、参数类型
 * 
 * @tparam T 函数类型
 */
template <typename T>
struct function_type_helper;

template <class T, class... U>
struct function_type_helper<std::function<T(U...)>> final
{
    using return_type = typename std::decay<T>::type;
    using param_type  = std::tuple<typename std::decay<U>::type...>;
};

template <typename T>
struct function_type_helper final
{
    using return_type = typename function_type_helper<decltype(
        std::function(std::declval<T>()))>::return_type;
    using param_type  = typename function_type_helper<decltype(
        std::function(std::declval<T>()))>::param_type;
};

template <typename Function, typename Tuple, std::size_t... Index>
decltype(auto) invoke_impl(Function&& func, Tuple&& t,
                           std::index_sequence<Index...>);

template <typename Function, typename Tuple>
decltype(auto) invoke(Function&& func, Tuple&& t);

template <typename _Type, typename Tuple, std::size_t... Index>
_Type* make_obj_from_tuple_impl(Tuple&& t, std::index_sequence<Index...>);

template <typename _Type, typename Tuple>
_Type* make_obj_from_tuple(Tuple&& t);

//////////////////////////////////////////////////////////////////////

template <typename Function, typename Tuple, std::size_t... Index>
decltype(auto) invoke_impl(Function&& func, Tuple&& t,
                           std::index_sequence<Index...>);

template <typename Function, typename Tuple>
decltype(auto) invoke(Function&& func, Tuple&& t);

template <typename _Type, typename Tuple, std::size_t... Index>
_Type* make_obj_from_tuple_impl(Tuple&& t, std::index_sequence<Index...>);

template <typename _Type, typename Tuple>
_Type* make_obj_from_tuple(Tuple&& t);

/////////////////////////////////////////////////////////
// make_placeholders

/**
 * @brief 占位符类型提取生成
 * 
 * @tparam _Placeholders 占位符类型
 */

template <template <int> typename _Placeholders>
struct __placeholders_type
{
    template <int N>
    __placeholders_type(_Placeholders<N>) {};
    template <int N>
    using type = _Placeholders<N>;
};

template <template <int> typename _Placeholders, int N>
__placeholders_type(_Placeholders<N>) -> __placeholders_type<_Placeholders>;

template <int N>
auto make_placeholders();

/**
 * @brief 判断类型是否是智能指针
 * 
 * @tparam T 类型
 */
template <typename T>
class is_shared_ptr : public std::false_type
{
};

template <typename T>
class is_shared_ptr<std::shared_ptr<T>> : public std::true_type
{
};

} // namespace skyfire
