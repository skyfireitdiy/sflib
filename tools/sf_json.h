
#pragma once
#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCUnusedGlobalDeclarationInspection"
#pragma ide diagnostic ignored "google-explicit-constructor"

#include "core/sf_define.h"
#include "core/sf_stdc++.h"
#include "sf_json_utils.h"

namespace skyfire {
/**
 * @brief json库
 */
class sf_json final {
   private:
    std::shared_ptr<sf_json_value> value__;

    explicit sf_json(const std::shared_ptr<sf_json_value> &value);

    void value_copy__(const std::shared_ptr<sf_json_value> &src,
                      std::shared_ptr<sf_json_value> &dst) const;

   public:
    /**
     * json字符串转为标准字符串
     * @param json_str json字符串
     * @return 标准字符串
     */
    static std::string json_string_to_string(std::string json_str);

    /**
     * 标准字符串转为json字符串
     * @param str 标准字符串
     * @return json字符串
     */
    static std::string string_to_json_string(std::string str);

    /**
     * 默认构造函数，null类型
     */
    sf_json();

    /**
     * 使用字符串构造一个json字符串对象
     * @param str 标准字符串
     */
    explicit sf_json(const std::string &str);

    /**
     * 使用c字符串构造一个json字符串对象
     * @param c_str c字符串
     */
    explicit sf_json(const char *c_str);

    /**
     * 使用json对象构造一个json对象
     * @param json
     */
    sf_json(const sf_json &json);

    /**
     * 使用bool值构造一个json对象
     * @param boolean_value bool值
     */
    explicit sf_json(bool boolean_value);

    /**
     * 转为object
     */
    void convert_to_object() const;

    /**
     * 转为array
     */
    void convert_to_array() const;

    /**
     * 转为null
     */
    void convert_to_null() const;

    /**
     * 获取类型
     * @return 类型
     */
    sf_json_type type() const;

    /**
     * 转为字符串
     * @return json字符串
     */
    std::string to_string() const;

    /**
     * 从字符串生成json对象
     * @param str json字符串
     * @return json对象
     */
    static sf_json from_string(const std::string &str);

    /**
     * 获取对象值
     * @param key 键
     * @return 值
     */
    sf_json at(const std::string &key) const;

    /**
     * 获取对象值
     * @param key 键
     * @return 值
     */
    sf_json operator[](const std::string &key) const;

    /**
     * 获取对象值
     * @param c_key 键
     * @return 值
     */
    sf_json at(const char *c_key) const;

    /**
     * 获取对象值
     * @param c_key 键
     * @return 值
     */
    sf_json operator[](const char *c_key) const;

    /**
     * 获取数组值
     * @param key 索引
     * @return 值
     */
    sf_json at(int key) const;

    /**
     * 获取数组值
     * @param key 索引
     * @return 值
     */
    sf_json operator[](int key) const;

    /**
     * 转换为字符串
     * @return 字符串
     */
    explicit operator std::string() const;

    /**
     * 转换为bool
     * @return bool值
     */
    operator bool() const;

    /**
     * 构造函数（算术类型）
     * @tparam T 类型
     * @param number 数字
     */
    template <typename T,
              typename = std::enable_if_t<std::is_arithmetic_v<T>, void>>
    sf_json(T number);

    /**
     * 隐式转换（算术类型）
     * @tparam T 类型
     * @return 数字
     */
    template <typename T,
              typename = std::enable_if_t<std::is_arithmetic_v<T>, void>>
    operator T() const;

    /**
     * 赋值（算术类型）
     * @tparam T 类型
     * @param value 值
     * @return sf_json对象
     */
    template <typename T,
              typename = std::enable_if_t<std::is_arithmetic_v<T>, void>>
    sf_json &operator=(T value);

    /**
     * 赋值运算符
     * @param value 值
     * @return json对象
     */
    sf_json &operator=(bool value);

    /**
     * 赋值运算符
     * @param value 值
     * @return json对象
     */
    sf_json &operator=(const std::string &value);

    /**
     * 赋值运算符
     * @param value 值
     * @return json对象
     */
    sf_json &operator=(const char *value);

    /**
     * 赋值运算符
     * @param value 值
     * @return json对象
     */
    sf_json &operator=(const sf_json &value);

    /**
     * 数组追加元素
     * @param value 元素值
     */
    void append(const sf_json &value) const;

    /**
     * 深拷贝，生成副本
     * @return 副本
     */
    sf_json deep_copy() const;

    /**
     * 合并（object或者array）
     * @param other 另一个object或者array
     * @return 是否合并成功
     */
    bool join(const sf_json &other) const;

    /**
     * 清空（不会改变type）
     */
    void clear() const;

    /**
     * 修改数组大小
     * @param size 大小
     */
    void resize(int size) const;

    /**
     * 获取数组大小
     * @return 数组大小
     */
    size_t size() const;

    /**
     * 是否有某个键
     * @param key 键
     * @return 是否存在
     */
    bool has(const std::string &key) const;

    /**
     * 是否有某个键
     * @param c_key 键
     * @return 是否存在
     */
    bool has(const char *c_key) const;

    /**
     * 删除数组元素
     * @param pos 位置
     */
    void remove(int pos) const;

    /**
     * 删除数组元素
     * @param pos 位置
     * @param len 长度
     */
    void remove(int pos, int len) const;

    /**
     * 删除对象元素
     * @param key 键
     */
    void remove(const std::string &key) const;

    /**
     * 是否为null
     * @return 是否为null
     */
    bool is_null() const;

    /**
     * 返回object的key列表
     * @return key列表
     */
    std::unordered_set<std::string> keys() const;

    /**
     * @brief 浅拷贝
     *
     * @param src 要拷贝的来源
     */
    void copy(const sf_json &src);
};

/**
 * 流输出
 * @param os 流
 * @param json json对象
 * @return 流
 */
std::ostream &operator<<(std::ostream &os, const sf_json &json);

/**
 * json字面值操作符
 * @param str 字符串
 * @return 长度
 */
sf_json operator""_json(const char *str, std::size_t);

/**
 * 转为json
 * @tparam T 类型
 * @param t 对象
 * @return json对象
 */
template <typename T>
sf_json to_json(const T &t);

/**
 * 转为json
 * @tparam T 类型
 * @param pt 对象
 * @return json对象
 */
template <typename T>
sf_json to_json(std::shared_ptr<T> pt);

/**
 * 从json加载对象
 * @tparam T 对象类型
 * @param js json对象
 * @param value 对象
 */

template <typename T>
void from_json(const sf_json &js, T &value);

/**
 * 从json加载对象
 * @tparam T 对象类型
 * @param js json对象
 * @param value 对象
 */
template <typename T>
void from_json(const sf_json &js, std::shared_ptr<T> &value);

/**
 * 转换pair为json
 * @tparam K 键类型
 * @tparam V 值类型
 * @param value 对象
 * @return json对象
 */
template <typename K, typename V>
sf_json to_json(const std::pair<K, V> &value);

/**
 * 从json加载对象
 * @tparam K
 * @tparam V
 * @param js
 * @param value
 */
template <typename K, typename V>
void from_json(const sf_json &js, std::pair<K, V> &value);

/**
 * 转换tuple对象为json
 * @tparam ARGS tuple类型
 * @param value 对象
 * @return json对象
 */
template <typename... ARGS>
sf_json to_json(const std::tuple<ARGS...> &value);

/**
 * 从json加载tuple对象
 * @tparam ARGS tuple类型
 * @param js json对象
 * @param value 对象
 */
template <typename... ARGS>
void from_json(const sf_json &js, std::tuple<ARGS...> &value);

template <int N, typename... ARGS, typename... Ret>
std::enable_if_t<N != sizeof...(ARGS), void> from_json_tuple_helper__(
    const sf_json &js, std::tuple<ARGS...> &data, Ret... ret);

template <int N, int, typename... ARGS, typename... Ret>
std::enable_if_t<N == sizeof...(ARGS), void> from_json_tuple_helper__(
    const sf_json &js, std::tuple<ARGS...> &data, Ret... ret);

template <typename... ARGS>
sf_json to_json_tuple_helper__(const ARGS &... value);

#define SF_CONTAINER_JSON_EXTERN(container)                 \
    template <typename T>                                   \
    sf_json to_json(const container<T> &value);             \
    template <typename T>                                   \
    void from_json(const sf_json &js, container<T> &value); \
    template <>                                             \
    sf_json to_json(const container<char> &value);          \
    template <>                                             \
    void from_json(const sf_json &js, container<char> &value);

#define SF_ASSOCIATED_CONTAINER_JSON_EXTERN(container) \
    template <typename K, typename V>                  \
    sf_json to_json(const container<K, V> &value);     \
    template <typename K, typename V>                  \
    void from_json(const sf_json &js, container<K, V> &value);

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
sf_json sf_to_json_helper__(const std::string &key, const T &value);

template <typename T, typename... ARGS>
sf_json sf_to_json_helper__(const std::string &key, const T &value,
                            const ARGS &... args);

template <typename T>
void sf_from_json_helper__(const sf_json &js, const std::string &key, T &value);

template <typename T, typename... ARGS>
void sf_from_json_helper__(const sf_json &js, const std::string &key, T &value,
                           ARGS &&... args);

}    // namespace skyfire

#define SF_JSONIFY(ClassName, ...)                                      \
    inline skyfire::sf_json to_json(const ClassName &obj) {             \
        return skyfire::sf_to_json_helper__(                            \
            SF_EXPAND_OBJ_MEM_WITH_NAME(obj, __VA_ARGS__));             \
    }                                                                   \
    inline void from_json(const skyfire::sf_json &js, ClassName &obj) { \
        skyfire::sf_from_json_helper__(                                 \
            js, SF_EXPAND_OBJ_MEM_WITH_NAME(obj, __VA_ARGS__));         \
    }

#pragma clang diagnostic pop