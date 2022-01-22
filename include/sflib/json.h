
#pragma once
#include "define.h"
#include "json_utils.h"
#include "stdc++.h"
#include "utils.h"
namespace skyfire
{
class json final
{
private:
    std::shared_ptr<json_value> value__;
    explicit json(const std::shared_ptr<json_value>& value);
    void value_copy__(const std::shared_ptr<json_value>& src,
                      std::shared_ptr<json_value>&       dst) const;

public:
    static std::string json_string_to_string(std::string json_str);
    static std::string string_to_json_string(std::string str);
    json();
    explicit json(const std::string& str);
    explicit json(const char* c_str);
    json(const json& js);
    explicit json(bool boolean_value);
    void        convert_to_object() const;
    void        convert_to_array() const;
    void        convert_to_null() const;
    json_type   type() const;
    std::string to_string() const;
    std::string to_string(int indent, int current_indent = 0) const;
    static json from_string(const std::string& str);
    json        at(const std::string& key) const;
    json        operator[](const std::string& key) const;
    json        at(const char* c_key) const;
    json        operator[](const char* c_key) const;
    json        at(size_t key) const;
    json        operator[](size_t key) const;
    explicit    operator std::string() const;
                operator bool() const;
    template <typename T,
              typename = std::enable_if_t<std::is_arithmetic_v<T>, void>>
    json(T number);
    template <typename T,
              typename = std::enable_if_t<std::is_arithmetic_v<T>, void>>
    operator T() const;
    template <typename T,
              typename = std::enable_if_t<std::is_arithmetic_v<T>, void>>
    json&                           operator=(T value);
    json&                           operator=(bool value);
    json&                           operator=(const std::string& value);
    json&                           operator=(const char* value);
    json&                           operator=(const json& value);
    void                            append(const json& value) const;
    json                            deep_copy() const;
    bool                            join(const json& other) const;
    void                            clear() const;
    void                            resize(size_t size) const;
    size_t                          size() const;
    bool                            has(const std::string& key) const;
    bool                            has(const char* c_key) const;
    void                            remove(size_t pos) const;
    void                            remove(size_t pos, size_t len) const;
    void                            remove(const std::string& key) const;
    bool                            is_null() const;
    std::unordered_set<std::string> keys() const;
    void                            copy(const json& src);
    bool                            operator==(const json& other) const;
    bool                            operator!=(const json& other) const;
};
std::ostream& operator<<(std::ostream& os, const json& json);
json          operator""_json(const char* str, std::size_t);
template <typename T>
json to_json(const T& t);
template <typename T>
json to_json(std::shared_ptr<T> pt);
template <typename T>
void from_json(const json& js, T& value);
template <typename T>
void from_json(const json& js, std::shared_ptr<T>& value);
template <typename K, typename V>
json to_json(const std::pair<K, V>& value);
template <typename K, typename V>
void from_json(const json& js, std::pair<K, V>& value);
template <typename... ARGS>
json to_json(const std::tuple<ARGS...>& value);
template <typename... ARGS>
void from_json(const json& js, std::tuple<ARGS...>& value);
template <int N, typename... ARGS, typename... Ret>
std::enable_if_t<N != sizeof...(ARGS), void> from_json_tuple_helper__(
    const json& js, std::tuple<ARGS...>& data, Ret... ret);
template <int N, int, typename... ARGS, typename... Ret>
std::enable_if_t<N == sizeof...(ARGS), void> from_json_tuple_helper__(
    const json& js, std::tuple<ARGS...>& data, Ret... ret);
template <typename... ARGS>
json to_json_tuple_helper__(const ARGS&... value);
#define SF_CONTAINER_JSON_EXTERN(container)              \
    template <typename T>                                \
    json to_json(const container<T>& value);             \
    template <typename T>                                \
    void from_json(const json& js, container<T>& value); \
    template <>                                          \
    json to_json(const container<char>& value);          \
    template <>                                          \
    void from_json(const json& js, container<char>& value);
#define SF_ASSOCIATED_CONTAINER_JSON_EXTERN(container) \
    template <typename K, typename V>                  \
    json to_json(const container<K, V>& value);        \
    template <typename K, typename V>                  \
    void from_json(const json& js, container<K, V>& value);
SF_CONTAINER_JSON_EXTERN(std::vector)
SF_CONTAINER_JSON_EXTERN(std::list)
SF_CONTAINER_JSON_EXTERN(std::deque)
SF_CONTAINER_JSON_EXTERN(std::set)
SF_CONTAINER_JSON_EXTERN(std::multiset)
SF_CONTAINER_JSON_EXTERN(std::unordered_set)
SF_CONTAINER_JSON_EXTERN(std::unordered_multiset)
SF_ASSOCIATED_CONTAINER_JSON_EXTERN(std::map)
SF_ASSOCIATED_CONTAINER_JSON_EXTERN(std::multimap)
SF_ASSOCIATED_CONTAINER_JSON_EXTERN(std::unordered_map)
SF_ASSOCIATED_CONTAINER_JSON_EXTERN(std::unordered_multimap)
#undef SF_CONTAINER_JSON_EXTERN
#undef SF_ASSOCIATED_CONTAINER_JSON_EXTERN
template <typename T>
json to_json_helper__(const std::string& key, const T& value);
template <typename T, typename... ARGS>
json to_json_helper__(const std::string& key, const T& value,
                      const ARGS&... args);
template <typename T>
void from_json_helper__(const json& js, const std::string& key, T& value);
template <typename T, typename... ARGS>
void from_json_helper__(const json& js, const std::string& key, T& value,
                        ARGS&&... args);
template <typename T, typename>
json& json::operator=(T value)
{
    if (value__->type != json_type::number)
    {
        value__->type = json_type::number;
        clear();
    }
    value__->value = std::to_string(value);
    return *this;
}

template <typename T>
json to_json(const T& t)
{
    return json(t);
}
template <typename T>
json to_json(std::shared_ptr<T> pt)
{
    return json(*pt);
}

template <typename T, typename>
json::operator T() const
{
    switch (value__->type)
    {
    case json_type::array:
    case json_type::object:
    case json_type::null:
    case json_type::string:
        return 0;
    case json_type::boolean:
        return value__->value == "true";
    case json_type::number:
        return static_cast<T>(string_to_long_double(value__->value));
    default:
        return 0;
    }
}
template <typename T, typename>
json::json(T number)
    : json()
{
    value__->type  = json_type::number;
    value__->value = long_double_to_string(number);
}
template <typename T>
json to_json_helper__(const std::string& key, const T& value)
{
    json js;
    js.convert_to_object();
    js[key] = to_json(value);
    return js;
}
template <typename T, typename... ARGS>
json to_json_helper__(const std::string& key, const T& value,
                      const ARGS&... args)
{
    json js;
    js.convert_to_object();
    js[key] = to_json(value);
    js.join(to_json_helper__(args...));
    return js;
}
template <typename T>
void from_json_helper__(const json& js, const std::string& key,
                        T& value)
{
    from_json(js.at(key), value);
}
template <typename T, typename... ARGS>
void from_json_helper__(const json& js, const std::string& key, T& value,
                        ARGS&&... args)
{
    from_json(js.at(key), value);
    from_json_helper__(js, std::forward<ARGS>(args)...);
}
template <typename... ARGS>
void from_json(const json& js, std::tuple<ARGS...>& value)
{
    from_json_tuple_helper__<0>(js, value);
}
template <typename T>
void from_json(const json& js, T& value)
{
    if (js.is_null())
    {
        return;
    }
    value = static_cast<T>(js);
}
template <typename T>
void from_json(const json& js, std::shared_ptr<T>& value)
{
    if (js.is_null())
    {
        return;
    }
    value = std::make_shared<T>(static_cast<T>(js));
}
template <typename K, typename V>
json to_json(const std::pair<K, V>& value)
{
    json js;
    js.convert_to_object();
    js["key"]   = to_json(value.first);
    js["value"] = to_json(value.second);
    return js;
}
template <typename K, typename V>
void from_json(const json& js, std::pair<K, V>& value)
{
    K k;
    V v;
    from_json(js.at("key", k));
    from_json(js.at("value", v));
    value = { k, v };
}
template <typename... ARGS>
json to_json(const std::tuple<ARGS...>& value)
{
    return invoke(to_json_tuple_helper__<ARGS...>, value);
}
template <typename... ARGS>
json to_json_tuple_helper__(const ARGS&... value)
{
    json js;
    js.convert_to_array();
    (js.append(to_json(value)), ...);
    return js;
}
template <int N, typename... ARGS, typename... Ret>
std::enable_if_t<N != sizeof...(ARGS), void> from_json_tuple_helper__(
    const json& js, std::tuple<ARGS...>& data, Ret... ret)
{
    std::remove_reference_t<decltype(std::get<N>(data))> t_data;
    from_json(js.at(N), t_data);
    from_json_tuple_helper__<N + 1>(js, data, ret..., t_data);
}
template <int N, typename... ARGS, typename... Ret>
std::enable_if_t<N == sizeof...(ARGS), void> from_json_tuple_helper__(
    const json& js, std::tuple<ARGS...>& data, Ret... ret)
{
    data = { ret... };
}

#define SF_CONTAINER_JSON_IMPL(container)                         \
    template <typename T>                                         \
    json to_json(const container<T>& value)                       \
    {                                                             \
        json js;                                                  \
        js.convert_to_array();                                    \
        for (auto& p : value)                                     \
        {                                                         \
            js.append(to_json(p));                                \
        }                                                         \
        return js;                                                \
    }                                                             \
    template <>                                                   \
    inline json to_json(const container<char>& value)             \
    {                                                             \
        return json(char_container_to_hex_string(value));         \
    }                                                             \
    template <typename T>                                         \
    void from_json(const json& js, container<T>& value)           \
    {                                                             \
        std::vector<T> data;                                      \
        int            sz = js.size();                            \
        data.resize(sz);                                          \
        for (int i = 0; i < sz; ++i)                              \
        {                                                         \
            from_json<T>(js.at(i), data[i]);                      \
        }                                                         \
        value = container<T> { data.begin(), data.end() };        \
    }                                                             \
    template <>                                                   \
    inline void from_json(const json& js, container<char>& value) \
    {                                                             \
        std::string tmp;                                          \
        from_json(js, tmp);                                       \
        hex_string_to_char_container(tmp, value);                 \
    }
#define SF_ASSOCIATED_CONTAINER_JSON_IMPL(container)                 \
    template <typename K, typename V>                                \
    json to_json(const container<K, V>& value)                       \
    {                                                                \
        json js;                                                     \
        js.convert_to_array();                                       \
        for (auto& p : value)                                        \
        {                                                            \
            json tmp_js;                                             \
            tmp_js.convert_to_object();                              \
            tmp_js["key"]   = to_json(p.first);                      \
            tmp_js["value"] = to_json(p.second);                     \
            js.append(tmp_js);                                       \
        }                                                            \
        return js;                                                   \
    }                                                                \
    template <typename K, typename V>                                \
    void from_json(const json& js, container<K, V>& value)           \
    {                                                                \
        std::vector<std::pair<K, V>> data;                           \
        int                          sz = js.size();                 \
        for (int i = 0; i < sz; ++i)                                 \
        {                                                            \
            data.push_back(                                          \
                std::make_pair(from_json<K>(js.at(i).at("key")),     \
                               from_json<V>(js.at(i).at("value")))); \
        }                                                            \
        value = container<K, V> { data.begin(), data.end() };        \
    }
SF_CONTAINER_JSON_IMPL(std::vector)
SF_CONTAINER_JSON_IMPL(std::list)
SF_CONTAINER_JSON_IMPL(std::deque)
SF_CONTAINER_JSON_IMPL(std::set)
SF_CONTAINER_JSON_IMPL(std::multiset)
SF_CONTAINER_JSON_IMPL(std::unordered_set)
SF_CONTAINER_JSON_IMPL(std::unordered_multiset)
SF_ASSOCIATED_CONTAINER_JSON_IMPL(std::map)
SF_ASSOCIATED_CONTAINER_JSON_IMPL(std::multimap)
SF_ASSOCIATED_CONTAINER_JSON_IMPL(std::unordered_map)
SF_ASSOCIATED_CONTAINER_JSON_IMPL(std::unordered_multimap)
#undef SF_CONTAINER_JSON_IMPL
#undef SF_ASSOCIATED_CONTAINER_JSON_IMPL

} // namespace skyfire
#define SF_JSONIFY(ClassName, ...)                                 \
    inline skyfire::json to_json(const ClassName& obj)             \
    {                                                              \
        return skyfire::to_json_helper__(                          \
            SF_EXPAND_OBJ_MEM_WITH_NAME(obj, __VA_ARGS__));        \
    }                                                              \
    inline void from_json(const skyfire::json& js, ClassName& obj) \
    {                                                              \
        skyfire::from_json_helper__(                               \
            js, SF_EXPAND_OBJ_MEM_WITH_NAME(obj, __VA_ARGS__));    \
    }
