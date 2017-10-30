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

namespace skyfire
{

using byte_array = std::vector<char>;

template <typename _Pod_Type>
typename std::enable_if<std::is_pod<_Pod_Type>::value, byte_array>::type serialize(const _Pod_Type &value);
template <typename _Pod_Type>
typename std::enable_if<std::is_pod<_Pod_Type>::value, size_t>::type
deserialize(const byte_array &data, _Pod_Type &obj, size_t begin_pos = 0);
template <typename _Type>
byte_array serialize(const std::vector<_Type> &value);
template <typename _Type>
size_t deserialize(const byte_array &data, std::vector<_Type> &obj, size_t begin_pos = 0);
template <typename _Type>
byte_array serialize(const std::list<_Type> &value);
template <typename _Type>
size_t deserialize(const byte_array &data, std::list<_Type> &obj, size_t begin_pos = 0);
template <typename _Type>
byte_array serialize(const std::deque<_Type> &value);
template <typename _Type>
size_t deserialize(const byte_array &data, std::deque<_Type> &obj, size_t begin_pos = 0);
template <typename _Type>
byte_array serialize(const std::set<_Type> &value);
template <typename _Type>
size_t deserialize(const byte_array &data, std::set<_Type> &obj, size_t begin_pos = 0);
template <typename _Type>
byte_array serialize(const std::unordered_set<_Type> &value);
template <typename _Type>
size_t deserialize(const byte_array &data, std::unordered_set<_Type> &obj, size_t begin_pos = 0);
template <typename _Type>
byte_array serialize(const std::multiset<_Type> &value);
template <typename _Type>
size_t deserialize(const byte_array &data, std::multiset<_Type> &obj, size_t begin_pos = 0);
template <typename _Type>
byte_array serialize(const std::basic_string<_Type> &value);
template <typename _Type>
size_t deserialize(const byte_array &data, std::basic_string<_Type> &obj, size_t begin_pos = 0);
template <typename _Type>
byte_array serialize(const std::unordered_multiset<_Type> &value);
template <typename _Type>
size_t deserialize(const byte_array &data, std::unordered_multiset<_Type> &obj, size_t begin_pos = 0);
template <typename _TypeKey, typename _TypeValue>
byte_array serialize(const std::unordered_multimap<_TypeKey, _TypeValue> &obj);
template <typename _TypeKey, typename _TypeValue>
size_t
deserialize(const byte_array &data, std::unordered_multimap<_TypeKey, _TypeValue> &obj, size_t begin_pos = 0);
template <typename _TypeKey, typename _TypeValue>
byte_array serialize(const std::unordered_map<_TypeKey, _TypeValue> &obj);
template <typename _TypeKey, typename _TypeValue>
size_t
deserialize(const byte_array &data, std::unordered_map<_TypeKey, _TypeValue> &obj, size_t begin_pos = 0);
template <typename _TypeKey, typename _TypeValue>
byte_array serialize(const std::multimap<_TypeKey, _TypeValue> &obj);
template <typename _TypeKey, typename _TypeValue>
size_t
deserialize(const byte_array &data, std::multimap<_TypeKey, _TypeValue> &obj, size_t begin_pos = 0);
template <typename _TypeKey, typename _TypeValue>
byte_array serialize(const std::map<_TypeKey, _TypeValue> &obj);
template <typename _TypeKey, typename _TypeValue>
size_t
deserialize(const byte_array &data, std::map<_TypeKey, _TypeValue> &obj, size_t begin_pos = 0);
template <typename _First_Type, typename... _Types>
byte_array serialize(const _First_Type &first, const _Types &... value);
template <int N, typename... _Types>
typename std::enable_if<N == sizeof...(_Types), byte_array>::type serialize_tuple(const std::tuple<_Types...> &obj);
template <int N, typename... _Types>
typename std::enable_if<N != sizeof...(_Types), byte_array>::type serialize_tuple(const std::tuple<_Types...> &obj);
template <typename... _Types>
byte_array serialize(const std::tuple<_Types...> &obj);
template <int N, typename _Tuple_Type, typename... _Types>
typename std::enable_if<N == std::tuple_size<_Tuple_Type>::value, size_t>::type deserialize_tuple(size_t pos, const byte_array &data, _Tuple_Type &obj, _Types... arg);
template <int N, typename _Tuple_Type, typename... _Types>
typename std::enable_if<N != std::tuple_size<_Tuple_Type>::value, size_t>::type deserialize_tuple(size_t pos, const byte_array &data, _Tuple_Type &obj, _Types... arg);
template <typename _Tuple_Type>
size_t deserialize_tuple(size_t pos, const byte_array &data, _Tuple_Type &obj);
template <typename... _Types>
size_t deserialize(const byte_array &data, std::tuple<_Types...> &obj, size_t begin_pos = 0);

byte_array operator+(byte_array b1, const byte_array &b2);
byte_array &operator+=(byte_array &b1, const byte_array &b2);

class serialize_size_mismatch_exception : public std::exception
{
  public:
    serialize_size_mismatch_exception(const std::string &exception_message) : _message(exception_message)
    {
    }

    const char *what() const noexcept override
    {
        return _message.c_str();
    }

  private:
    std::string _message;
};

template <typename _Pod_Type>
typename std::enable_if<std::is_pod<_Pod_Type>::value, byte_array>::type serialize(const _Pod_Type &value)
{
    byte_array ret(sizeof(_Pod_Type));
    auto p = reinterpret_cast<const char *>(&value);
    memcpy(ret.data(), &value, sizeof(_Pod_Type));
    return ret;
}

template <typename _Pod_Type>
typename std::enable_if<std::is_pod<_Pod_Type>::value, size_t>::type
deserialize(const byte_array &data, _Pod_Type &obj, size_t begin_pos)
{
    if (sizeof(_Pod_Type) > data.size() - begin_pos)
    {
        throw serialize_size_mismatch_exception("Data size is to small");
    }
    memcpy(&obj, data.data() + begin_pos, sizeof(_Pod_Type));
    return begin_pos + sizeof(_Pod_Type);
}

template <typename _Type>
byte_array serialize(const std::vector<_Type> &value)
{
    byte_array ret;
    size_t len = value.size();
    auto tmp_ret = serialize(len);
    ret.insert(ret.end(), tmp_ret.begin(), tmp_ret.end());
    for (auto const &p : value)
    {
        tmp_ret = serialize(p);
        ret.insert(ret.end(), tmp_ret.begin(), tmp_ret.end());
    }
    return ret;
}

template <typename _Type>
size_t deserialize(const byte_array &data, std::vector<_Type> &obj,
                   size_t begin_pos)
{
    obj.clear();
    size_t len;
    auto offset = deserialize(data, len, begin_pos);
    obj.resize(len);
    for (auto i = 0; i < len; ++i)
    {
        offset = deserialize(data, obj[i], offset);
    }
    return offset;
}

template <typename _Type>
byte_array serialize(const std::list<_Type> &value)
{
    std::vector<_Type> tmp_obj(value.begin(), value.end());
    return serialize(tmp_obj);
}

template <typename _Type>
size_t deserialize(const byte_array &data, std::list<_Type> &obj,
                   size_t begin_pos)
{
    std::vector<_Type> tmp_obj;
    auto ret = deserialize(data, tmp_obj, begin_pos);
    obj = std::list<_Type>(tmp_obj.begin(), tmp_obj.end());
    return ret;
}

template <typename _Type>
byte_array serialize(const std::deque<_Type> &value)
{
    std::vector<_Type> tmp_obj(value.begin(), value.end());
    return serialize(tmp_obj);
}

template <typename _Type>
size_t deserialize(const byte_array &data, std::deque<_Type> &obj,
                   size_t begin_pos)
{
    std::vector<_Type> tmp_obj;
    auto ret = deserialize(data, tmp_obj, begin_pos);
    obj = std::deque<_Type>(tmp_obj.begin(), tmp_obj.end());
    return ret;
}

template <typename _Type>
byte_array serialize(const std::set<_Type> &value)
{
    std::vector<_Type> tmp_obj(value.begin(), value.end());
    return serialize(tmp_obj);
}

template <typename _Type>
size_t deserialize(const byte_array &data, std::set<_Type> &obj,
                   size_t begin_pos)
{
    std::vector<_Type> tmp_obj;
    auto ret = deserialize(data, tmp_obj, begin_pos);
    obj = std::set<_Type>(tmp_obj.begin(), tmp_obj.end());
    return ret;
}

template <typename _Type>
byte_array serialize(const std::unordered_set<_Type> &value)
{
    std::vector<_Type> tmp_obj(value.begin(), value.end());
    return serialize(tmp_obj);
}

template <typename _Type>
size_t deserialize(const byte_array &data, std::unordered_set<_Type> &obj,
                   size_t begin_pos)
{
    std::vector<_Type> tmp_obj;
    auto ret = deserialize(data, tmp_obj, begin_pos);
    obj = std::unordered_set<_Type>(tmp_obj.begin(), tmp_obj.end());
    return ret;
}

template <typename _Type>
byte_array serialize(const std::multiset<_Type> &value)
{
    std::vector<_Type> tmp_obj(value.begin(), value.end());
    return serialize(tmp_obj);
}

template <typename _Type>
size_t deserialize(const byte_array &data, std::multiset<_Type> &obj,
                   size_t begin_pos)
{
    std::vector<_Type> tmp_obj;
    auto ret = deserialize(data, tmp_obj, begin_pos);
    obj = std::multiset<_Type>(tmp_obj.begin(), tmp_obj.end());
    return ret;
}

template <typename _Type>
byte_array serialize(const std::basic_string<_Type> &value)
{
    std::vector<char> tmp_obj(value.begin(), value.end());
    return serialize(tmp_obj);
}

template <typename _Type>
size_t deserialize(const byte_array &data, std::basic_string<_Type> &obj,
                   size_t begin_pos)
{
    std::vector<char> tmp_obj;
    auto ret = deserialize(data, tmp_obj, begin_pos);
    obj = std::basic_string<_Type>(tmp_obj.begin(), tmp_obj.end());
    return ret;
}

template <typename _Type>
byte_array serialize(const std::unordered_multiset<_Type> &value)
{
    std::vector<_Type> tmp_obj(value.begin(), value.end());
    return serialize(tmp_obj);
}

template <typename _Type>
size_t deserialize(const byte_array &data, std::unordered_multiset<_Type> &obj,
                   size_t begin_pos)
{
    std::vector<_Type> tmp_obj;
    auto ret = deserialize(data, tmp_obj, begin_pos);
    obj = std::unordered_multiset<_Type>(tmp_obj.begin(), tmp_obj.end());
    return ret;
}

template <typename _TypeKey, typename _TypeValue>
byte_array serialize(const std::unordered_multimap<_TypeKey, _TypeValue> &obj)
{
    byte_array ret;
    size_t len = obj.size();
    auto tmp_ret = serialize(len);
    ret.insert(ret.end(), tmp_ret.begin(), tmp_ret.end());
    for (auto const &p : obj)
    {
        tmp_ret = serialize(p.first);
        ret.insert(ret.end(), tmp_ret.begin(), tmp_ret.end());
        tmp_ret = serialize(p.second);
        ret.insert(ret.end(), tmp_ret.begin(), tmp_ret.end());
    }
    return ret;
}

template <typename _TypeKey, typename _TypeValue>
size_t
deserialize(const byte_array &data, std::unordered_multimap<_TypeKey, _TypeValue> &obj,
            size_t begin_pos)
{
    obj.clear();
    size_t len;
    auto offset = deserialize(data, len, begin_pos);
    _TypeKey key;
    _TypeValue value;
    for (auto i = 0; i < len; ++i)
    {
        offset = deserialize(data, key, offset);
        offset = deserialize(data, value, offset);
        obj.insert({key, value});
    }
    return offset;
}

template <typename _TypeKey, typename _TypeValue>
byte_array serialize(const std::unordered_map<_TypeKey, _TypeValue> &obj)
{
    std::unordered_multimap<_TypeKey, _TypeValue> tmp_obj(obj.begin(), obj.end());
    return serialize(tmp_obj);
}

template <typename _TypeKey, typename _TypeValue>
size_t
deserialize(const byte_array &data, std::unordered_map<_TypeKey, _TypeValue> &obj,
            size_t begin_pos)
{
    std::unordered_multimap<_TypeKey, _TypeValue> tmp_obj;
    auto ret = deserialize(data, tmp_obj, begin_pos);
    obj = std::unordered_map<_TypeKey, _TypeValue>(tmp_obj.begin(), tmp_obj.end());
    return ret;
}

template <typename _TypeKey, typename _TypeValue>
byte_array serialize(const std::multimap<_TypeKey, _TypeValue> &obj)
{
    std::unordered_multimap<_TypeKey, _TypeValue> tmp_obj(obj.begin(), obj.end());
    return serialize(tmp_obj);
}

template <typename _TypeKey, typename _TypeValue>
size_t
deserialize(const byte_array &data, std::multimap<_TypeKey, _TypeValue> &obj,
            size_t begin_pos)
{
    std::unordered_multimap<_TypeKey, _TypeValue> tmp_obj;
    auto ret = deserialize(data, tmp_obj, begin_pos);
    obj = std::multimap<_TypeKey, _TypeValue>(tmp_obj.begin(), tmp_obj.end());
    return ret;
}

template <typename _TypeKey, typename _TypeValue>
byte_array serialize(const std::map<_TypeKey, _TypeValue> &obj)
{
    std::unordered_multimap<_TypeKey, _TypeValue> tmp_obj(obj.begin(), obj.end());
    return serialize(tmp_obj);
}

template <typename _TypeKey, typename _TypeValue>
size_t
deserialize(const byte_array &data, std::map<_TypeKey, _TypeValue> &obj,
            size_t begin_pos)
{
    std::unordered_multimap<_TypeKey, _TypeValue> tmp_obj;
    auto ret = deserialize(data, tmp_obj, begin_pos);
    obj = std::map<_TypeKey, _TypeValue>(tmp_obj.begin(), tmp_obj.end());
    return ret;
}

template <typename _First_Type, typename... _Types>
byte_array serialize(const _First_Type &first, const _Types &... value)
{
    return serialize(first) + serialize(value...);
}

template <int N, typename... _Types>
typename std::enable_if<N == sizeof...(_Types), byte_array>::type serialize_tuple(const std::tuple<_Types...> &obj)
{
    return byte_array();
}

template <int N, typename... _Types>
typename std::enable_if<N != sizeof...(_Types), byte_array>::type serialize_tuple(const std::tuple<_Types...> &obj)
{
    return serialize(std::get<N>(obj)) + serialize_tuple<N + 1, _Types...>(obj);
}

template <typename... _Types>
byte_array serialize(const std::tuple<_Types...> &obj)
{
    return serialize_tuple<0, _Types...>(obj);
}

template <int N, typename _Tuple_Type, typename... _Types>
typename std::enable_if<N == std::tuple_size<_Tuple_Type>::value, size_t>::type deserialize_tuple(size_t pos, const byte_array &data, _Tuple_Type &obj, _Types... arg)
{
    obj = std::tuple<_Types...>(arg...);
    return pos;
}

template <int N, typename _Tuple_Type, typename... _Types>
typename std::enable_if<N != std::tuple_size<_Tuple_Type>::value, size_t>::type deserialize_tuple(size_t pos, const byte_array &data, _Tuple_Type &obj, _Types... arg)
{
    using next_type = typename std::remove_reference<decltype(std::get<N>(std::declval<_Tuple_Type>()))>::type;
    next_type _t;
    pos = deserialize(data, _t, pos);
    return deserialize_tuple<N + 1, _Tuple_Type, _Types..., next_type>(pos, data, obj, arg..., _t);
}

template <typename _Tuple_Type>
size_t deserialize_tuple(size_t pos, const byte_array &data, _Tuple_Type &obj)
{
    using next_type = typename std::remove_reference<decltype(std::get<0>(std::declval<_Tuple_Type>()))>::type;
    next_type _t;
    pos = deserialize(data, _t, pos);
    return deserialize_tuple<1, _Tuple_Type, next_type>(pos, data, obj, _t);
}

template <typename... _Types>
size_t deserialize(const byte_array &data, std::tuple<_Types...> &obj, size_t begin_pos)
{
    return deserialize_tuple(begin_pos, data, obj);
}

byte_array operator+(byte_array b1, const byte_array &b2);
byte_array &operator+=(byte_array &b1, const byte_array &b2);
}