
/**
* @version 1.0.0
* @author skyfire
* @mail skyfireitdiy@hotmail.com
* @see http://github.com/skyfireitdiy/sflib
* @file sf_serialize_binary.hpp

* sflib第一版本发布
* 版本号1.0.0
* 发布日期：2018-10-22
*/

/*
 * 提供序列化支持（二进制）
 */

#pragma once

#include "sf_serialize_binary.h"

namespace skyfire {


    inline sf_serialize_binary_size_mismatch_exception::
    sf_serialize_binary_size_mismatch_exception(const std::string &exception_message) : _message(exception_message) {
    }

    inline const char *sf_serialize_binary_size_mismatch_exception::what() const noexcept {
        return _message.c_str();
    }

    inline byte_array sf_serialize_binary() {
        return byte_array();
    }

    template<typename _Pod_Type>
    typename std::enable_if<std::is_pod<_Pod_Type>::value, byte_array>::type
    sf_serialize_binary(const _Pod_Type &value) {
        byte_array ret(sizeof(_Pod_Type));
        memcpy(ret.data(), &value, sizeof(_Pod_Type));
        return ret;
    }

    template<typename _Pod_Type>
    typename std::enable_if<std::is_pod<_Pod_Type>::value, size_t>::type
    sf_deserialize_binary(const byte_array &data, _Pod_Type &obj, size_t begin_pos) {
        if (sizeof(_Pod_Type) > data.size() - begin_pos) {
            throw sf_serialize_binary_size_mismatch_exception("Data size is to small");
        }
        memcpy(&obj, data.data() + begin_pos, sizeof(_Pod_Type));
        return begin_pos + sizeof(_Pod_Type);
    }


#define SF_CONTAINTER_SERIALIZE_BINARY_IMPL(container)\
    template<typename _Type>\
    byte_array sf_serialize_binary(const container<_Type> &value) {\
        std::vector<_Type> tmp_obj(value.begin(), value.end());\
        return sf_serialize_binary(tmp_obj);\
    }\
    template<typename _Type>\
    size_t sf_deserialize_binary(const byte_array &data, container<_Type> &obj,\
                                 size_t begin_pos) {\
        std::vector<_Type> tmp_obj;\
        auto ret = sf_deserialize_binary(data, tmp_obj, begin_pos);\
        obj = container<_Type>(tmp_obj.begin(), tmp_obj.end());\
        return ret;\
    }\


#define SF_ASSOCIATED_CONTAINTER_SERIALIZE_BINARY_IMPL(container) \
    template<typename _TypeKey, typename _TypeValue>\
    byte_array sf_serialize_binary(const container<_TypeKey, _TypeValue> &obj) {\
        byte_array ret;\
        size_t len = obj.size();\
        auto tmp_ret = sf_serialize_binary(len);\
        ret.insert(ret.end(), tmp_ret.begin(), tmp_ret.end());\
        for (auto const &p : obj) {\
            tmp_ret = sf_serialize_binary(p.first);\
            ret.insert(ret.end(), tmp_ret.begin(), tmp_ret.end());\
            tmp_ret = sf_serialize_binary(p.second);\
            ret.insert(ret.end(), tmp_ret.begin(), tmp_ret.end());\
        }\
        return ret;\
    }\
    template<typename _TypeKey, typename _TypeValue>\
    size_t\
    sf_deserialize_binary(const byte_array &data, container<_TypeKey, _TypeValue> &obj,\
                          size_t begin_pos) {\
        obj.clear();\
        size_t len;\
        auto offset = sf_deserialize_binary(data, len, begin_pos);\
        _TypeKey key;\
        _TypeValue value;\
        std::vector<std::pair<_TypeKey,_TypeValue>> tmp_data;\
        for (auto i = 0; i < len; ++i) {\
            offset = sf_deserialize_binary(data, key, offset);\
            offset = sf_deserialize_binary(data, value, offset);\
            tmp_data.emplace_back({key, value});\
        }\
        obj = container<_TypeKey, _TypeValue>{tmp_data.begin(), tmp_data.end()};\
        return offset;\
    }\


    SF_CONTAINTER_SERIALIZE_BINARY_IMPL(std::vector)
    SF_CONTAINTER_SERIALIZE_BINARY_IMPL(std::list)
    SF_CONTAINTER_SERIALIZE_BINARY_IMPL(std::deque)
    SF_CONTAINTER_SERIALIZE_BINARY_IMPL(std::queue)
    SF_CONTAINTER_SERIALIZE_BINARY_IMPL(std::set)
    SF_CONTAINTER_SERIALIZE_BINARY_IMPL(std::multiset)
    SF_CONTAINTER_SERIALIZE_BINARY_IMPL(std::unordered_set)
    SF_CONTAINTER_SERIALIZE_BINARY_IMPL(std::unordered_multiset)
    SF_CONTAINTER_SERIALIZE_BINARY_IMPL(std::basic_string)

    SF_ASSOCIATED_CONTAINTER_SERIALIZE_BINARY_IMPL(std::map)
    SF_ASSOCIATED_CONTAINTER_SERIALIZE_BINARY_IMPL(std::multimap)
    SF_ASSOCIATED_CONTAINTER_SERIALIZE_BINARY_IMPL(std::unordered_map)
    SF_ASSOCIATED_CONTAINTER_SERIALIZE_BINARY_IMPL(std::unordered_multimap)


#undef SF_CONTAINTER_SERIALIZE_BINARY_IMPL
#undef SF_ASSOCIATED_CONTAINTER_SERIALIZE_BINARY_IMPL

    template<typename _First_Type, typename... _Types>
    byte_array sf_serialize_binary(const _First_Type &first, const _Types &... value) {
        return sf_serialize_binary(first) + sf_serialize_binary(value...);
    }

    template<int N, typename... _Types>
    typename std::enable_if<N == sizeof...(_Types), byte_array>::type
    sf_serialize_binary_tuple(const std::tuple<_Types...> &obj) {
        return byte_array();
    }

    template<int N, typename... _Types>
    typename std::enable_if<N != sizeof...(_Types), byte_array>::type
    sf_serialize_binary_tuple(const std::tuple<_Types...> &obj) {
        return sf_serialize_binary(std::get<N>(obj)) + sf_serialize_binary_tuple<N + 1, _Types...>(obj);
    }

    template<typename... _Types>
    byte_array sf_serialize_binary(const std::tuple<_Types...> &obj) {
        return sf_serialize_binary_tuple<0, _Types...>(obj);
    }

    template<int N, typename _Tuple_Type, typename... _Types>
    typename std::enable_if<N == std::tuple_size<_Tuple_Type>::value, size_t>::type
    sf_deserialize_binary_tuple(size_t pos, const byte_array &data, _Tuple_Type &obj, _Types... arg) {
        obj = std::tuple<_Types...>(arg...);
        return pos;
    }

    template<int N, typename _Tuple_Type, typename... _Types>
    typename std::enable_if<N != std::tuple_size<_Tuple_Type>::value, size_t>::type
    sf_deserialize_binary_tuple(size_t pos, const byte_array &data, _Tuple_Type &obj, _Types... arg) {
        using next_type = typename std::remove_reference<decltype(std::get<N>(std::declval<_Tuple_Type>()))>::type;
        next_type _t;
        pos = sf_deserialize_binary(data, _t, pos);
        return sf_deserialize_binary_tuple<N + 1, _Tuple_Type, _Types..., next_type>(pos, data, obj, arg..., _t);
    }

    template<typename _Tuple_Type>
    size_t sf_deserialize_binary_tuple(size_t pos, const byte_array &data, _Tuple_Type &obj) {
        using next_type = typename std::remove_reference<decltype(std::get<0>(std::declval<_Tuple_Type>()))>::type;
        next_type _t;
        pos = sf_deserialize_binary(data, _t, pos);
        return sf_deserialize_binary_tuple<1, _Tuple_Type, next_type>(pos, data, obj, _t);
    }

    template<typename... _Types>
    size_t sf_deserialize_binary(const byte_array &data, std::tuple<_Types...> &obj, size_t begin_pos) {
        if constexpr (sizeof...(_Types) == 0) {
            return begin_pos;
        } else {
            return sf_deserialize_binary_tuple(begin_pos, data, obj);
        }
    }

    template<typename ... _Type>
    byte_array sf_serialize_binary_obj_helper(const _Type &... obj) {
        return (sf_serialize_binary(obj) + ...);
    }

    template<typename T>
    size_t sf_deserialize_binary_obj_helper(const byte_array &data, size_t begin_pos, T &obj) {
        return sf_deserialize_binary(data, obj, begin_pos);
    }

    template<typename T, typename ... _Type>
    size_t sf_deserialize_binary_obj_helper(const byte_array &data, size_t begin_pos, T &obj, _Type &... other) {
        auto pos = sf_deserialize_binary(data, obj, begin_pos);
        return sf_deserialize_binary_obj_helper(data, pos, other...);
    }

}


