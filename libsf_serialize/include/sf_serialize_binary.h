
/**
* @version 1.0.0
* @author skyfire
* @email skyfireitdiy@hotmail.com
* @see http://github.com/skyfireitdiy/sflib
* @file sf_serialize_binary.h

* sflib第一版本发布
* 版本号1.0.0
* 发布日期：2018-10-22
*/

/*
 * 提供序列化支持（二进制）
 */

#pragma once

#include <type_traits>
#include <vector>
#include <exception>
#include <string>
#include <cstring>
#include <list>
#include <deque>
#include <set>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <tuple>
#include "sf_type.hpp"
#include "sf_define.h"

namespace skyfire
{
    template<typename _Pod_Type>
    typename std::enable_if<std::is_pod<_Pod_Type>::value, byte_array>::type sf_serialize_binary(const _Pod_Type &value);

    template<typename _Pod_Type>
    typename std::enable_if<std::is_pod<_Pod_Type>::value, size_t>::type
    sf_deserialize_binary(const byte_array &data, _Pod_Type &obj, size_t begin_pos);

    template<typename _Type>
    byte_array sf_serialize_binary(const std::vector <_Type> &value);

    template<typename _Type>
    size_t sf_deserialize_binary(const byte_array &data, std::vector <_Type> &obj, size_t begin_pos);

    template<typename _Type>
    byte_array sf_serialize_binary(const std::list <_Type> &value);

    template<typename _Type>
    size_t sf_deserialize_binary(const byte_array &data, std::list <_Type> &obj, size_t begin_pos);

    template<typename _Type>
    byte_array sf_serialize_binary(const std::deque <_Type> &value);

    template<typename _Type>
    size_t sf_deserialize_binary(const byte_array &data, std::deque <_Type> &obj, size_t begin_pos);

    template<typename _Type>
    byte_array sf_serialize_binary(const std::set <_Type> &value);

    template<typename _Type>
    size_t sf_deserialize_binary(const byte_array &data, std::set <_Type> &obj, size_t begin_pos);

    template<typename _Type>
    byte_array sf_serialize_binary(const std::unordered_set <_Type> &value);

    template<typename _Type>
    size_t sf_deserialize_binary(const byte_array &data, std::unordered_set <_Type> &obj, size_t begin_pos);

    template<typename _Type>
    byte_array sf_serialize_binary(const std::multiset <_Type> &value);

    template<typename _Type>
    size_t sf_deserialize_binary(const byte_array &data, std::multiset <_Type> &obj, size_t begin_pos);

    template<typename _Type>
    byte_array sf_serialize_binary(const std::basic_string <_Type> &value);

    template<typename _Type>
    size_t sf_deserialize_binary(const byte_array &data, std::basic_string <_Type> &obj, size_t begin_pos);

    template<typename _Type>
    byte_array sf_serialize_binary(const std::unordered_multiset <_Type> &value);

    template<typename _Type>
    size_t sf_deserialize_binary(const byte_array &data, std::unordered_multiset <_Type> &obj, size_t begin_pos);

    template<typename _TypeKey, typename _TypeValue>
    byte_array sf_serialize_binary(const std::unordered_multimap <_TypeKey, _TypeValue> &obj);

    template<typename _TypeKey, typename _TypeValue>
    size_t
    sf_deserialize_binary(const byte_array &data, std::unordered_multimap <_TypeKey, _TypeValue> &obj, size_t begin_pos);

    template<typename _TypeKey, typename _TypeValue>
    byte_array sf_serialize_binary(const std::unordered_map <_TypeKey, _TypeValue> &obj);

    template<typename _TypeKey, typename _TypeValue>
    size_t
    sf_deserialize_binary(const byte_array &data, std::unordered_map <_TypeKey, _TypeValue> &obj, size_t begin_pos);

    template<typename _TypeKey, typename _TypeValue>
    byte_array sf_serialize_binary(const std::multimap <_TypeKey, _TypeValue> &obj);

    template<typename _TypeKey, typename _TypeValue>
    size_t
    sf_deserialize_binary(const byte_array &data, std::multimap <_TypeKey, _TypeValue> &obj, size_t begin_pos);

    template<typename _TypeKey, typename _TypeValue>
    byte_array sf_serialize_binary(const std::map <_TypeKey, _TypeValue> &obj);

    template<typename _TypeKey, typename _TypeValue>
    size_t
    sf_deserialize_binary(const byte_array &data, std::map <_TypeKey, _TypeValue> &obj, size_t begin_pos);

    template<typename _First_Type, typename... _Types>
    byte_array sf_serialize_binary(const _First_Type &first, const _Types &... value);

    template<int N, typename... _Types>
    typename std::enable_if<N == sizeof...(_Types), byte_array>::type
    sf_serialize_binary_tuple(const std::tuple<_Types...> &obj);

    template<int N, typename... _Types>
    typename std::enable_if<N != sizeof...(_Types), byte_array>::type
    sf_serialize_binary_tuple(const std::tuple<_Types...> &obj);

    template<typename... _Types>
    byte_array sf_serialize_binary(const std::tuple<_Types...> &obj);

    template<int N, typename _Tuple_Type, typename... _Types>
    typename std::enable_if<N == std::tuple_size<_Tuple_Type>::value, size_t>::type
    sf_deserialize_binary_tuple(size_t pos, const byte_array &data, _Tuple_Type &obj, _Types... arg);

    template<int N, typename _Tuple_Type, typename... _Types>
    typename std::enable_if<N != std::tuple_size<_Tuple_Type>::value, size_t>::type
    sf_deserialize_binary_tuple(size_t pos, const byte_array &data, _Tuple_Type &obj, _Types... arg);

    template<typename _Tuple_Type>
    size_t sf_deserialize_binary_tuple(size_t pos, const byte_array &data, _Tuple_Type &obj);

    template<typename... _Types>
    size_t sf_deserialize_binary(const byte_array &data, std::tuple<_Types...> &obj, size_t begin_pos);

    class sf_serialize_binary_size_mismatch_exception : public std::exception
    {
    public:
        sf_serialize_binary_size_mismatch_exception(const std::string &exception_message);
        const char *what() const noexcept override;
    private:
        std::string _message;
    };


    template<typename T>
    size_t sf_deserialize_binary_obj_helper(const byte_array &data, size_t begin_pos, T &obj) ;

    template<typename T, typename ... _Type>
    size_t sf_deserialize_binary_obj_helper(const byte_array &data, size_t begin_pos, T &obj, _Type &... other);


    //使一个结构变成可序列化的结构（需保证内部的每个成员都可以序列化，使用时需要注入到skyfire命名空间内部）

#define SF_MAKE_SERIALIZABLE_BINARY(className, ...)                                                                            \
inline byte_array sf_serialize_binary(const className& obj){                                                                  \
        return sf_serialize_binary_obj_helper(SF_EXPAND_OBJ_MEM(obj, __VA_ARGS__));                                            \
    }                                                                                                                   \
    inline size_t                                                                                                       \
    sf_deserialize_binary(const byte_array &data, className &obj, size_t begin_pos){                                           \
        return sf_deserialize_binary_obj_helper(data, begin_pos, SF_EXPAND_OBJ_MEM(obj, __VA_ARGS__));                         \
    }                                                                                                                   \


}


