
/**
* @version 1.0.0
* @author skyfire
* @file sf_json.hpp

* 
*/

#pragma once
#pragma clang diagnostic push
#pragma ide diagnostic ignored "cppcoreguidelines-c-copy-assignment-signature"
#pragma ide diagnostic ignored "cert-err34-c"
#pragma ide diagnostic ignored "OCUnusedGlobalDeclarationInspection"

#include "sf_define.h"
#include "sf_error.h"
#include "sf_json.h"
#include "sf_lex.hpp"
#include "sf_logger.hpp"
#include "sf_meta.hpp"
#include "sf_msvc_safe.h"
#include "sf_utils.hpp"
#include "sf_yacc.hpp"

namespace skyfire {
inline json json::from_string(const std::string& json_str)
{
    lex lex;
    lex.set_rules(
        { { "string",
              R"("([^\\"]|(\\["\\bnrt]|(u[0-9a-fA-F][0-9a-fA-F][0-9a-fA-F][0-9a-fA-F])))*")" },
            { "[", R"(\[)" },
            { "]", R"(\])" },
            { "{", R"(\{)" },
            { "}", R"(\})" },
            { ",", R"(,)" },
            { ":", R"(:)" },
            { "ws", R"([\r\n\t ]+)" },
            { "number", R"(-?(0|[1-9]\d*)(\.\d+)?(e|E(\+|-)?0|[1-9]\d*)?)" },
            { "true", R"(true)" },
            { "false", R"(false)" },
            { "null", R"(null)" } });

    std::vector<lex_result_t> lex_result;
    if (!lex.parse(json_str, lex_result)) {
        return json();
    }
    lex_result.erase(
        std::remove_if(lex_result.begin(), lex_result.end(),
            [](const lex_result_t& r) { return r.id == "ws"; }),
        lex_result.end());
    yacc yacc;
    yacc.set_rules(
        { { "value",
              { { { "object" },
                    [](const std::vector<std::shared_ptr<yacc_result_t>>& data)
                        -> std::any { return data[0]->user_data; } },
                  { { "array" },
                      [](const std::vector<std::shared_ptr<yacc_result_t>>& data)
                          -> std::any { return data[0]->user_data; } },
                  { { "string" },
                      [](const std::vector<std::shared_ptr<yacc_result_t>>& data)
                          -> std::any {
                          return json(json::json_string_to_string(data[0]->text));
                      } },
                  { { "number" },
                      [](const std::vector<std::shared_ptr<yacc_result_t>>& data)
                          -> std::any {
                          return json(string_to_long_double(data[0]->text));
                      } },
                  { { "true" },
                      [](const std::vector<std::shared_ptr<yacc_result_t>>& data)
                          -> std::any { return json(true); } },
                  { { "false" },
                      [](const std::vector<std::shared_ptr<yacc_result_t>>& data)
                          -> std::any { return json(false); } },
                  { { "null" },
                      [](const std::vector<std::shared_ptr<yacc_result_t>>& data)
                          -> std::any { return json(); } } } },
            { "object",
                { { { "{", "}" },
                      [](const std::vector<std::shared_ptr<yacc_result_t>>& data)
                          -> std::any {
                          json json;
                          json.convert_to_object();
                          return json;
                      } },
                    { { "{", "members", "}" },
                        [](const std::vector<std::shared_ptr<yacc_result_t>>& data)
                            -> std::any { return data[1]->user_data; } } } },
            { "members",
                { { { "member" },
                      [](const std::vector<std::shared_ptr<yacc_result_t>>& data)
                          -> std::any { return data[0]->user_data; } },
                    { { "members", ",", "member" },
                        [](const std::vector<std::shared_ptr<yacc_result_t>>& data)
                            -> std::any {
                            json js;
                            js.convert_to_object();
                            const auto js1 = std::any_cast<json>(data[0]->user_data);
                            const auto js2 = std::any_cast<json>(data[2]->user_data);
                            // ReSharper disable once CppExpressionWithoutSideEffects
                            js.join(js1);
                            // ReSharper disable once CppExpressionWithoutSideEffects
                            js.join(js2);
                            return js;
                        } } } },
            { "member",
                { { { "string", ":", "value" },
                    [](const std::vector<std::shared_ptr<yacc_result_t>>& data)
                        -> std::any {
                        json js;
                        js.convert_to_object();
                        const auto json1 = std::any_cast<json>(data[2]->user_data);
                        js[json::json_string_to_string(data[0]->text)] = json1;
                        return js;
                    } } } },
            { "array",
                { { { "[", "]" },
                      [](const std::vector<std::shared_ptr<yacc_result_t>>& data)
                          -> std::any {
                          json json;
                          json.convert_to_array();
                          return json;
                      } },
                    { { "[", "values", "]" },
                        [](const std::vector<std::shared_ptr<yacc_result_t>>& data)
                            -> std::any { return data[1]->user_data; } } } },
            { "values",
                { { { "value" },
                      [](const std::vector<std::shared_ptr<yacc_result_t>>& data)
                          -> std::any {
                          json js;
                          js.convert_to_array();
                          js.append(std::any_cast<json>(data[0]->user_data));
                          return js;
                      } },
                    { { "values", ",", "value" },
                        [](const std::vector<std::shared_ptr<yacc_result_t>>& data)
                            -> std::any {
                            json js;
                            js.convert_to_array();
                            const auto json1 = std::any_cast<json>(data[0]->user_data);
                            const auto json2 = std::any_cast<json>(data[2]->user_data);
                            // ReSharper disable once CppExpressionWithoutSideEffects
                            js.join(json1);
                            js.append(json2);
                            return js;
                        } } } } });

    yacc.add_terminate_ids({ "value" });
    std::vector<std::shared_ptr<yacc_result_t>> yacc_result;
    if (!yacc.parse(lex_result, yacc_result)) {
        return json();
    }
    return std::any_cast<json>(yacc_result[0]->user_data);
}

inline std::unordered_set<std::string> json::keys() const
{
    std::unordered_set<std::string> ret;
    if (value__->type != json_type::object) {
        return ret;
    }
    for (auto& p : value__->object_value) {
        ret.insert(p.first);
    }
    return ret;
}

inline json::json()
    : value__(std::make_shared<json_value>())
{
    value__->type = json_type::null;
}

inline json::json(const std::string& str)
    : json()
{
    value__->type = json_type::string;
    value__->value = str;
}

inline json::json(const char* c_str)
    : json(std::string(c_str))
{
}

inline json::json(const json& js)
    : json()
{
    value__ = js.value__;
}

inline json::json(bool boolean_value)
    : json()
{
    value__->type = json_type::boolean;
    value__->value = boolean_value ? "true" : "false";
}

inline json::json(const std::shared_ptr<json_value>& value)
{
    value__ = value;
}

inline void json::convert_to_object() const
{
    if (value__->type == json_type::object) {
        return;
    }
    value__->type = json_type::object;
    clear();
}

inline void json::convert_to_array() const
{
    if (value__->type == json_type::array) {
        return;
    }
    value__->type = json_type::array;
    clear();
}

inline json_type json::type() const { return value__->type; }

inline json json::at(const std::string& key) const
{
    if (value__->type != json_type::object) {
        return json();
    }
    if (value__->object_value.count(key) == 0) {
        return json();
    }
    return json(value__->object_value[key]);
}

inline json json::operator[](const std::string& key) const
{
    if (value__->type != json_type::object) {
        convert_to_object();
    }
    if (value__->object_value.count(key) == 0) {
        value__->object_value[key] = json().value__;
    }
    return json(value__->object_value[key]);
}

inline json json::at(int key) const
{
    if (value__->type != json_type::array) {
        return json();
    }
    if (value__->array_value.size() <= key) {
        return json();
    }
    return json(value__->array_value[key]);
}

inline json::operator std::string() const
{
    switch (value__->type) {
    case json_type::array:
    case json_type::object:
    case json_type::null:
        return "";
    case json_type::boolean:
    case json_type::number:
    case json_type::string:
        return value__->value;
    default:
        return "";
    }
}

inline std::string json::json_string_to_string(std::string json_str)
{
    json_str.erase(json_str.begin());
    json_str.pop_back();
    string_replace(json_str, "\\\"", "\"");
    // string_replace(json_str, "\\/", "/");
    string_replace(json_str, "\\b", "\b");
    string_replace(json_str, "\\f", "\f");
    string_replace(json_str, "\\n", "\n");
    string_replace(json_str, "\\r", "\r");
    string_replace(json_str, "\\t", "\t");

    // todo 未处理unicode

    string_replace(json_str, "\\\\", "\\");

    return json_str;
}

inline std::string json::string_to_json_string(std::string str)
{
    string_replace(str, "\\", "\\\\");
    string_replace(str, "\"", "\\\"");
    // string_replace(str, "/", "\\/");
    string_replace(str, "\b", "\\b");
    string_replace(str, "\f", "\\f");
    string_replace(str, "\n", "\\n");
    string_replace(str, "\r", "\\r");
    string_replace(str, "\t", "\\t");
    // todo 未处理unicode
    str.insert(str.begin(), '\"');
    str += "\"";
    return str;
}

inline json::operator bool() const
{
    switch (value__->type) {
    case json_type::array:
        return !value__->array_value.empty();
    case json_type::object:
        return !value__->object_value.empty();
    case json_type::null:
        return false;
    case json_type::boolean:
        return value__->value == "true";
    case json_type::number:
        return string_to_long_double(value__->value) != 0;
    case json_type::string:
        return !value__->value.empty();
    default:
        return false;
    }
}

template <typename T, typename>
json& json::operator=(T value)
{
    if (value__->type != json_type::number) {
        value__->type = json_type::number;
        clear();
    }
    value__->value = std::to_string(value);
    return *this;
}

inline json& json::operator=(const std::string& value)
{
    if (value__->type != json_type::string) {
        value__->type = json_type::string;
        clear();
    }
    value__->value = value;
    return *this;
}

inline void json::append(const json& value) const
{
    convert_to_array();
    value__->array_value.push_back(value.value__);
}

inline json json::deep_copy() const
{
    json tmp_json;
    value_copy__(value__, tmp_json.value__);
    return tmp_json;
}

inline bool json::join(const json& other) const
{
    if (value__->type == json_type::array && other.value__->type == json_type::array) {
        value__->array_value.insert(value__->array_value.end(),
            other.value__->array_value.begin(),
            other.value__->array_value.end());
        return true;
    } else if (value__->type == json_type::object && other.value__->type == json_type::object) {
        for (auto& p : other.value__->object_value) {
            value__->object_value[p.first] = p.second;
        }
        return true;
    }
    return false;
}

inline void json::clear() const
{
    value__->value.clear();
    value__->array_value.clear();
    value__->object_value.clear();
}

inline void json::resize(const int size) const
{
    convert_to_array();
    value__->array_value.resize((unsigned long)size);
}

inline void json::remove(const int pos) const
{
    if (value__->array_value.size() > pos) {
        value__->array_value.erase(value__->array_value.begin() + pos);
    }
}

inline void json::remove(const int pos, int len) const
{
    if (value__->array_value.size() > pos) {
        if (len > value__->array_value.size() - pos) {
            len = static_cast<int>(value__->array_value.size() - pos);
        }
        value__->array_value.erase(value__->array_value.begin() + pos,
            value__->array_value.begin() + pos + len);
    }
}

inline void json::remove(const std::string& key) const
{
    value__->object_value.erase(key);
}

inline std::string json::to_string() const
{
    std::string ret;
    switch (value__->type) {
    case json_type::object: {
        ret += "{";
        for (auto& p : value__->object_value) {
            ret += string_to_json_string(p.first) + ":" + json(p.second).to_string() + ",";
        }
        if (ret.back() == ',')
            ret.pop_back();
        ret += "}";
    } break;
    case json_type::array: {
        ret += "[";
        for (auto& p : value__->array_value) {
            ret += json(p).to_string() + ",";
        }
        if (ret.back() == ',')
            ret.pop_back();
        ret += "]";
    } break;
    case json_type::string:
        ret += string_to_json_string(value__->value);
        break;
    case json_type::number: {
        ret += value__->value;
        break;
    }
    case json_type::boolean:
        ret += value__->value.empty() ? "false" : value__->value;
        break;
    case json_type::null:
        ret += "null";
        break;
    }
    return ret;
}

inline std::string json::to_string(int indent, int current_indent) const
{
    std::string indent_string(current_indent, ' ');
    std::string ret;
    switch (value__->type) {
    case json_type::object: {
        ret += "{\n";
        for (auto& p : value__->object_value) {
            ret += indent_string + std::string(indent, ' ') + string_to_json_string(p.first) + ":" + json(p.second).to_string(indent, current_indent + indent) + ",\n";
        }
        if (!value__->object_value.empty()) {
            ret.erase(ret.end() - 2);
        }
        ret += indent_string + "}";
    } break;
    case json_type::array: {
        ret += "[\n";
        for (auto& p : value__->array_value) {
            ret += indent_string + std::string(indent, ' ') + json(p).to_string(indent, current_indent + indent) + ",\n";
        }
        if (!value__->object_value.empty()) {
            ret.erase(ret.end() - 2);
        }
        ret += indent_string + "]";
    } break;
    case json_type::string:
        ret += string_to_json_string(value__->value);
        break;
    case json_type::number: {
        ret += value__->value;
        break;
    }
    case json_type::boolean:
        ret += (value__->value.empty() ? "false" : value__->value);
        break;
    case json_type::null:
        ret += "null";
        break;
    }
    return ret;
}

inline json& json::operator=(const json& value)
{
    if (&value != this) {
        value__->type = value.value__->type;
        value__->value = value.value__->value;
        value__->array_value = value.value__->array_value;
        value__->object_value = value.value__->object_value;
    }
    return *this;
}

inline void json::copy(const json& src)
{
    if (this != &src) {
        value__ = src.value__;
    }
}

inline void json::value_copy__(const std::shared_ptr<json_value>& src,
    std::shared_ptr<json_value>& dst) const
{
    dst->type = src->type;
    dst->value = src->value;
    if (!src->array_value.empty()) {
        for (auto i = 0; i < src->array_value.size(); ++i) {
            dst->array_value.push_back(std::make_shared<json_value>());
            value_copy__(src->array_value[i], dst->array_value[i]);
        }
    }
    if (!src->object_value.empty()) {
        for (auto& p : src->object_value) {
            dst->object_value[p.first] = std::make_shared<json_value>();
            value_copy__(p.second, dst->object_value[p.first]);
        }
    }
}

inline json json::at(const char* c_key) const
{
    return at(std::string(c_key));
}

inline json json::operator[](const char* c_key) const
{
    return operator[](std::string(c_key));
}

inline json json::operator[](const int key) const
{
    if (value__->type != json_type::array) {
        convert_to_array();
    }
    if (value__->array_value.size() <= key) {
        resize(key + 1);
    }
    return json(value__->array_value[key]);
}

inline json& json::operator=(const bool value)
{
    if (value__->type != json_type::boolean) {
        value__->type = json_type::boolean;
        clear();
    }
    value__->value = value ? "true" : "false";
    return *this;
}

inline bool json::is_null() const
{
    return value__->type == json_type::null;
}

inline json& json::operator=(const char* value)
{
    return operator=(std::string(value));
}

inline void json::convert_to_null() const
{
    clear();
    value__->type = json_type ::null;
}

inline size_t json::size() const
{
    return value__->type == json_type ::array ? value__->array_value.size()
                                              : 0;
}

inline bool json::has(const std::string& key) const
{
    return value__->type == json_type ::object
        ? value__->object_value.count(key) != 0
        : false;
}

inline bool json::has(const char* c_key) const
{
    return has(std::string(c_key));
}

inline json operator""_json(const char* str, std::size_t)
{
    return json::from_string(str);
}

inline std::ostream& operator<<(std::ostream& os, const json& json)
{
    return os << json.to_string();
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

inline bool json::operator!=(const json& other) const
{
    return !(*this == other);
}

inline bool json::operator==(const json& other) const
{
    if (value__->type != other.value__->type) {
        return false;
    }
    switch (value__->type) {
    case json_type::null:
        return true;
    case json_type::array: {
        auto sz = size();
        auto osz = other.size();
        if (sz != osz) {
            return false;
        }
        for (int i = 0; i < sz; ++i) {
            if ((*this)[i] != other[i]) {
                return false;
            }
        }
        return true;
    }
    case json_type::boolean: {
        return bool(*this) == bool(other);
    }
    case json_type::number: {
        return static_cast<long double>(*this) == static_cast<long double>(other);
    }
    case json_type::object: {
        auto this_keys = keys();
        auto other_keys = other.keys();
        if (this_keys != other_keys) {
            return false;
        }
        for (auto p : this_keys) {
            if ((*this)[p] != other[p]) {
                return false;
            }
        }
        return true;
    }
    case json_type::string: {
        return static_cast<std::string>(*this) == static_cast<std::string>(other);
    }
    default:
        throw exception(err_unsupport_type, "unsupport type:" + std::to_string(static_cast<int>(value__->type)));
    }
}

template <typename T, typename>
json::operator T() const
{
    switch (value__->type) {
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
    value__->type = json_type::number;
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
    if (js.is_null()) {
        return;
    }
    value = static_cast<T>(js);
}

template <typename T>
void from_json(const json& js, std::shared_ptr<T>& value)
{
    if (js.is_null()) {
        return;
    }
    value = std::make_shared<T>(static_cast<T>(js));
}

template <typename K, typename V>
json to_json(const std::pair<K, V>& value)
{
    json js;
    js.convert_to_object();
    js["key"] = to_json(value.first);
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
    inline json to_json(const container<T>& value)                \
    {                                                             \
        json js;                                                  \
        js.convert_to_array();                                    \
        for (auto& p : value) {                                   \
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
    inline void from_json(const json& js, container<T>& value)    \
    {                                                             \
        std::vector<T> data;                                      \
        int sz = js.size();                                       \
        data.resize(sz);                                          \
        for (int i = 0; i < sz; ++i) {                            \
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

#define SF_ASSOCIATED_CONTAINER_JSON_IMPL(container)             \
    template <typename K, typename V>                            \
    json to_json(const container<K, V>& value)                   \
    {                                                            \
        json js;                                                 \
        js.convert_to_array();                                   \
        for (auto& p : value) {                                  \
            json tmp_js;                                         \
            tmp_js.convert_to_object();                          \
            tmp_js["key"] = to_json(p.first);                    \
            tmp_js["value"] = to_json(p.second);                 \
            js.append(tmp_js);                                   \
        }                                                        \
        return js;                                               \
    }                                                            \
    template <typename K, typename V>                            \
    void from_json(const json& js, container<K, V>& value)       \
    {                                                            \
        std::vector<std::pair<K, V>> data;                       \
        int sz = js.size();                                      \
        for (int i = 0; i < sz; ++i) {                           \
            data.push_back(                                      \
                std::make_pair(from_json<K>(js.at(i).at("key")), \
                    from_json<V>(js.at(i).at("value"))));        \
        }                                                        \
        value = container<K, V> { data.begin(), data.end() };    \
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

#pragma clang diagnostic pop