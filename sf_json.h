#pragma once

#include "sf_json_utils.h"
#include "sf_lex.h"
#include "sf_yacc.h"
#include <ostream>

namespace skyfire
{
    /**
     * @brief json库
     */
    class sf_json
    {

    private:
        std::shared_ptr<sf_json_value> value__;

        sf_json(const std::shared_ptr<sf_json_value> &value);

        void value_copy__(const std::shared_ptr<sf_json_value> &src, std::shared_ptr<sf_json_value> &dst) const;

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
        sf_json(const std::string &str);

        /**
         * 使用c字符串构造一个json字符串对象
         * @param c_str c字符串
         */
        sf_json(const char *c_str);

        /**
         * 使用浮点数数字构造一个json数值对象
         * @param number 数字
         */
        sf_json(long double number);

        /**
         * 使用浮长整数数字构造一个json数值对象
         * @param number 数字
         */
        sf_json(long long number);

        /**
         * 使用整数数字构造一个json数值对象
         * @param number 数字
         */
        sf_json(int number);

        /**
         * 使用json对象构造一个json对象
         * @param json
         */
        sf_json(const sf_json &json);

        /**
         * 使用bool值构造一个json对象
         * @param boolean_value bool值
         */
        sf_json(bool boolean_value);

        /**
         * 转为object
         */
        void convert_to_object();

        /**
         * 转为array
         */
        void convert_to_array();

        /**
         * 转为null
         */
        void convert_to_null();

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
        const sf_json operator[](const std::string &key) const;

        /**
         * 获取对象值
         * @param key 键
         * @return 值
         */
        sf_json operator[](const std::string &key);

        /**
         * 获取对象值
         * @param c_key 键
         * @return 值
         */
        const sf_json operator[](const char *c_key) const;

        /**
         * 获取对象值
         * @param c_key 键
         * @return 值
         */
        sf_json operator[](const char *c_key);

        /**
         * 获取数组值
         * @param key 索引
         * @return 值
         */
        const sf_json operator[](int key) const;

        /**
         * 获取数组值
         * @param key 索引
         * @return 值
         */
        sf_json operator[](int key);

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
         * 转换为long double
         * @return long double值
         */
        operator long double() const;

        /**
         * 转换为long long
         * @return long long值
         */
        operator long long() const;

        /**
         * 转换为int
         * @return int值
         */
        operator int() const;

        /**
         * 赋值运算符
         * @param value 值
         * @return json对象
         */
        sf_json &operator=(long double value);

        /**
         * 赋值运算符
         * @param value 值
         * @return json对象
         */
        sf_json &operator=(long long value);

        /**
         * 赋值运算符
         * @param value 值
         * @return json对象
         */
        sf_json &operator=(int value);

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
        void append(const sf_json &value);

        /**
         * 深拷贝，生成副本
         * @return 副本
         */
        sf_json clone() const;

        /**
         * 合并（object或者array）
         * @param other 另一个object或者array
         * @return 是否合并成功
         */
        bool join(const sf_json &other);

        /**
         * 清空（不会改变type）
         */
        void clear();

        /**
         * 修改数组大小
         * @param size 大小
         */
        void resize(int size);

        /**
         * 获取数组大小
         * @return 数组大小
         */
        size_t size() const;

        /**
         * 是否有某个键
         * @param c_key 键
         * @return 是否存在
         */
        bool has(const std::string &key);

        /**
         * 是否有某个键
         * @param c_key 键
         * @return 是否存在
         */
        bool has(const char *c_key);

        /**
         * 删除数组元素
         * @param pos 位置
         */
        void remove(int pos);

        /**
         * 删除数组元素
         * @param pos 位置
         * @param len 长度
         */
        void remove(int pos, int len);

        /**
         * 删除对象元素
         * @param key 键
         */
        void remove(const std::string &key);

        /**
         * 是否为null
         * @return 是否为null
         */
        bool is_null();

        /**
         * 返回object的key列表
         * @return key列表
         */
        std::set<std::string> keys() const;
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
    sf_json operator ""_json(const char *str, std::size_t);

}