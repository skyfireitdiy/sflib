
/**
* @version 1.0.0
* @author skyfire
* @mail skyfireitdiy@hotmail.com
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

#include "sf_stdc++.h"
#include "sf_type.hpp"

namespace skyfire
{
    template<typename _Pod_Type>
    typename std::enable_if<std::is_pod<_Pod_Type>::value, byte_array>::type sf_serialize_binary(const _Pod_Type &value);

    template<typename _Pod_Type>
    typename std::enable_if<std::is_pod<_Pod_Type>::value, size_t>::type
    sf_deserialize_binary(const byte_array &data, _Pod_Type &obj, size_t begin_pos);


#define SF_CONTAINTER_SERIALIZE_BINARY_EXTERN(container)\
    template<typename _Type>\
    byte_array sf_serialize_binary(const container <_Type> &value);\
    template<typename _Type>\
    size_t sf_deserialize_binary(const byte_array &data, container <_Type> &obj, size_t begin_pos);\
    
    
#define SF_ASSOCIATED_CONTAINTER_SERIALIZE_BINARY_EXTERN(container)\
    template<typename _TypeKey, typename _TypeValue>\
    byte_array sf_serialize_binary(const container <_TypeKey, _TypeValue> &obj);\
    template<typename _TypeKey, typename _TypeValue>\
    size_t\
    sf_deserialize_binary(const byte_array &data, container <_TypeKey, _TypeValue> &obj, size_t begin_pos);\

    

    SF_CONTAINTER_SERIALIZE_BINARY_EXTERN(std::vector)
    SF_CONTAINTER_SERIALIZE_BINARY_EXTERN(std::list)
    SF_CONTAINTER_SERIALIZE_BINARY_EXTERN(std::deque)
    SF_CONTAINTER_SERIALIZE_BINARY_EXTERN(std::queue)
    SF_CONTAINTER_SERIALIZE_BINARY_EXTERN(std::set)
    SF_CONTAINTER_SERIALIZE_BINARY_EXTERN(std::multiset)
    SF_CONTAINTER_SERIALIZE_BINARY_EXTERN(std::unordered_set)
    SF_CONTAINTER_SERIALIZE_BINARY_EXTERN(std::unordered_multiset)
    SF_CONTAINTER_SERIALIZE_BINARY_EXTERN(std::basic_string)


    SF_ASSOCIATED_CONTAINTER_SERIALIZE_BINARY_EXTERN(std::map)
    SF_ASSOCIATED_CONTAINTER_SERIALIZE_BINARY_EXTERN(std::multimap)
    SF_ASSOCIATED_CONTAINTER_SERIALIZE_BINARY_EXTERN(std::unordered_map)
    SF_ASSOCIATED_CONTAINTER_SERIALIZE_BINARY_EXTERN(std::unordered_multimap)


#undef SF_CONTAINTER_SERIALIZE_BINARY_EXTERN
#undef SF_ASSOCIATED_CONTAINTER_SERIALIZE_BINARY_EXTERN


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

#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCUnusedGlobalDeclarationInspection"
    template<int N, typename _Tuple_Type, typename... _Types>
    typename std::enable_if<N != std::tuple_size<_Tuple_Type>::value, size_t>::type
    sf_deserialize_binary_tuple(size_t pos, const byte_array &data, _Tuple_Type &obj, _Types... arg);
#pragma clang diagnostic pop

    template<typename _Tuple_Type>
    size_t sf_deserialize_binary_tuple(size_t pos, const byte_array &data, _Tuple_Type &obj);

    template<typename... _Types>
    size_t sf_deserialize_binary(const byte_array &data, std::tuple<_Types...> &obj, size_t begin_pos);

    class sf_serialize_binary_size_mismatch_exception final : public std::exception
    {
    public:
	    explicit sf_serialize_binary_size_mismatch_exception(const std::string &exception_message);
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
inline skyfire::byte_array sf_serialize_binary(const className& obj){                                                                  \
        return skyfire::sf_serialize_binary_obj_helper(SF_EXPAND_OBJ_MEM(obj, __VA_ARGS__));                                            \
    }                                                                                                                   \
    inline size_t                                                                                                       \
    sf_deserialize_binary(const skyfire::byte_array &data, className &obj, size_t begin_pos){                                           \
        return skyfire::sf_deserialize_binary_obj_helper(data, begin_pos, SF_EXPAND_OBJ_MEM(obj, __VA_ARGS__));                         \
    }                                                                                                                   \


}


