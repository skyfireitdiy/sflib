
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
        auto p = reinterpret_cast<const char *>(&value);
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

    template<typename _Type>
    byte_array sf_serialize_binary(const std::vector<_Type> &value) {
        byte_array ret;
        size_t len = value.size();
        auto tmp_ret = sf_serialize_binary(len);
        ret.insert(ret.end(), tmp_ret.begin(), tmp_ret.end());
        for (auto const &p : value) {
            tmp_ret = sf_serialize_binary(p);
            ret.insert(ret.end(), tmp_ret.begin(), tmp_ret.end());
        }
        return ret;
    }

    template<typename _Type>
    size_t sf_deserialize_binary(const byte_array &data, std::vector<_Type> &obj,
                                 size_t begin_pos) {
        obj.clear();
        size_t len;
        auto offset = sf_deserialize_binary(data, len, begin_pos);
        obj.resize(len);
        for (auto i = 0; i < len; ++i) {
            offset = sf_deserialize_binary(data, obj[i], offset);
        }
        return offset;
    }

    template<typename _Type>
    byte_array sf_serialize_binary(const std::list<_Type> &value) {
        std::vector<_Type> tmp_obj(value.begin(), value.end());
        return sf_serialize_binary(tmp_obj);
    }

    template<typename _Type>
    size_t sf_deserialize_binary(const byte_array &data, std::list<_Type> &obj,
                                 size_t begin_pos) {
        std::vector<_Type> tmp_obj;
        auto ret = sf_deserialize_binary(data, tmp_obj, begin_pos);
        obj = std::list<_Type>(tmp_obj.begin(), tmp_obj.end());
        return ret;
    }

    template<typename _Type>
    byte_array sf_serialize_binary(const std::deque<_Type> &value) {
        std::vector<_Type> tmp_obj(value.begin(), value.end());
        return sf_serialize_binary(tmp_obj);
    }

    template<typename _Type>
    size_t sf_deserialize_binary(const byte_array &data, std::deque<_Type> &obj,
                                 size_t begin_pos) {
        std::vector<_Type> tmp_obj;
        auto ret = sf_deserialize_binary(data, tmp_obj, begin_pos);
        obj = std::deque<_Type>(tmp_obj.begin(), tmp_obj.end());
        return ret;
    }

    template<typename _Type>
    byte_array sf_serialize_binary(const std::set<_Type> &value) {
        std::vector<_Type> tmp_obj(value.begin(), value.end());
        return sf_serialize_binary(tmp_obj);
    }

    template<typename _Type>
    size_t sf_deserialize_binary(const byte_array &data, std::set<_Type> &obj,
                                 size_t begin_pos) {
        std::vector<_Type> tmp_obj;
        auto ret = sf_deserialize_binary(data, tmp_obj, begin_pos);
        obj = std::set<_Type>(tmp_obj.begin(), tmp_obj.end());
        return ret;
    }

    template<typename _Type>
    byte_array sf_serialize_binary(const std::unordered_set<_Type> &value) {
        std::vector<_Type> tmp_obj(value.begin(), value.end());
        return sf_serialize_binary(tmp_obj);
    }

    template<typename _Type>
    size_t sf_deserialize_binary(const byte_array &data, std::unordered_set<_Type> &obj,
                                 size_t begin_pos) {
        std::vector<_Type> tmp_obj;
        auto ret = sf_deserialize_binary(data, tmp_obj, begin_pos);
        obj = std::unordered_set<_Type>(tmp_obj.begin(), tmp_obj.end());
        return ret;
    }

    template<typename _Type>
    byte_array sf_serialize_binary(const std::multiset<_Type> &value) {
        std::vector<_Type> tmp_obj(value.begin(), value.end());
        return sf_serialize_binary(tmp_obj);
    }

    template<typename _Type>
    size_t sf_deserialize_binary(const byte_array &data, std::multiset<_Type> &obj,
                                 size_t begin_pos) {
        std::vector<_Type> tmp_obj;
        auto ret = sf_deserialize_binary(data, tmp_obj, begin_pos);
        obj = std::multiset<_Type>(tmp_obj.begin(), tmp_obj.end());
        return ret;
    }

    template<typename _Type>
    byte_array sf_serialize_binary(const std::basic_string<_Type> &value) {
        std::vector<char> tmp_obj(value.begin(), value.end());
        return sf_serialize_binary(tmp_obj);
    }

    template<typename _Type>
    size_t sf_deserialize_binary(const byte_array &data, std::basic_string<_Type> &obj,
                                 size_t begin_pos) {
        std::vector<char> tmp_obj;
        auto ret = sf_deserialize_binary(data, tmp_obj, begin_pos);
        obj = std::basic_string<_Type>(tmp_obj.begin(), tmp_obj.end());
        return ret;
    }

    template<typename _Type>
    byte_array sf_serialize_binary(const std::unordered_multiset<_Type> &value) {
        std::vector<_Type> tmp_obj(value.begin(), value.end());
        return sf_serialize_binary(tmp_obj);
    }

    template<typename _Type>
    size_t sf_deserialize_binary(const byte_array &data, std::unordered_multiset<_Type> &obj,
                                 size_t begin_pos) {
        std::vector<_Type> tmp_obj;
        auto ret = sf_deserialize_binary(data, tmp_obj, begin_pos);
        obj = std::unordered_multiset<_Type>(tmp_obj.begin(), tmp_obj.end());
        return ret;
    }

    template<typename _TypeKey, typename _TypeValue>
    byte_array sf_serialize_binary(const std::unordered_multimap<_TypeKey, _TypeValue> &obj) {
        byte_array ret;
        size_t len = obj.size();
        auto tmp_ret = sf_serialize_binary(len);
        ret.insert(ret.end(), tmp_ret.begin(), tmp_ret.end());
        for (auto const &p : obj) {
            tmp_ret = sf_serialize_binary(p.first);
            ret.insert(ret.end(), tmp_ret.begin(), tmp_ret.end());
            tmp_ret = sf_serialize_binary(p.second);
            ret.insert(ret.end(), tmp_ret.begin(), tmp_ret.end());
        }
        return ret;
    }

    template<typename _TypeKey, typename _TypeValue>
    size_t
    sf_deserialize_binary(const byte_array &data, std::unordered_multimap<_TypeKey, _TypeValue> &obj,
                          size_t begin_pos) {
        obj.clear();
        size_t len;
        auto offset = sf_deserialize_binary(data, len, begin_pos);
        _TypeKey key;
        _TypeValue value;
        for (auto i = 0; i < len; ++i) {
            offset = sf_deserialize_binary(data, key, offset);
            offset = sf_deserialize_binary(data, value, offset);
            obj.insert({key, value});
        }
        return offset;
    }

    template<typename _TypeKey, typename _TypeValue>
    byte_array sf_serialize_binary(const std::unordered_map<_TypeKey, _TypeValue> &obj) {
        std::unordered_multimap<_TypeKey, _TypeValue> tmp_obj(obj.begin(), obj.end());
        return sf_serialize_binary(tmp_obj);
    }

    template<typename _TypeKey, typename _TypeValue>
    size_t
    sf_deserialize_binary(const byte_array &data, std::unordered_map<_TypeKey, _TypeValue> &obj,
                          size_t begin_pos) {
        std::unordered_multimap<_TypeKey, _TypeValue> tmp_obj;
        auto ret = sf_deserialize_binary(data, tmp_obj, begin_pos);
        obj = std::unordered_map<_TypeKey, _TypeValue>(tmp_obj.begin(), tmp_obj.end());
        return ret;
    }

    template<typename _TypeKey, typename _TypeValue>
    byte_array sf_serialize_binary(const std::multimap<_TypeKey, _TypeValue> &obj) {
        std::unordered_multimap<_TypeKey, _TypeValue> tmp_obj(obj.begin(), obj.end());
        return sf_serialize_binary(tmp_obj);
    }

    template<typename _TypeKey, typename _TypeValue>
    size_t
    sf_deserialize_binary(const byte_array &data, std::multimap<_TypeKey, _TypeValue> &obj,
                          size_t begin_pos) {
        std::unordered_multimap<_TypeKey, _TypeValue> tmp_obj;
        auto ret = sf_deserialize_binary(data, tmp_obj, begin_pos);
        obj = std::multimap<_TypeKey, _TypeValue>(tmp_obj.begin(), tmp_obj.end());
        return ret;
    }

    template<typename _TypeKey, typename _TypeValue>
    byte_array sf_serialize_binary(const std::map<_TypeKey, _TypeValue> &obj) {
        std::unordered_multimap<_TypeKey, _TypeValue> tmp_obj(obj.begin(), obj.end());
        return sf_serialize_binary(tmp_obj);
    }

    template<typename _TypeKey, typename _TypeValue>
    size_t
    sf_deserialize_binary(const byte_array &data, std::map<_TypeKey, _TypeValue> &obj,
                          size_t begin_pos) {
        std::unordered_multimap<_TypeKey, _TypeValue> tmp_obj;
        auto ret = sf_deserialize_binary(data, tmp_obj, begin_pos);
        obj = std::map<_TypeKey, _TypeValue>(tmp_obj.begin(), tmp_obj.end());
        return ret;
    }

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


