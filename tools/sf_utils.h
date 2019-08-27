
/**
* @version 1.0.0
* @author skyfire
* @mail skyfireitdiy@hotmail.com
* @see http://github.com/skyfireitdiy/sflib
* @file sf_utils.h

* sflib第一版本发布
* 版本号1.0.0
* 发布日期：2018-10-22
*/

#pragma once

#include <sys/stat.h>
#include <sys/types.h>
#include <cstring>
#include <fstream>
#include <string>
#include "core/sf_empty_class.h"
#include "core/sf_type.hpp"

namespace skyfire {
/**
 * 字符串去除首尾空白
 * @param str 字符串
 * @return 处理后的字符串
 */
inline std::string sf_string_trim(const std::string &str);

/**
 * 字符串转小写
 * @param str 字符串
 * @return 处理后的字符串
 */
inline std::string sf_to_lower_string(std::string str);

/**
 * 获取文件后缀
 * @param path 路径
 * @return 后缀
 */
inline std::string sf_get_path_ext(const std::string &path);

/**
 * 不区分大小写比较字符串
 * @param str1 字符串1
 * @param str2 字符串2
 * @return 是否相等
 */
inline bool sf_equal_nocase_string(const std::string &str1,
                                   const std::string &str2);

/**
 * 分割字符串
 * @param str 原始字符串
 * @param split_str 分隔符
 * @return 分割后的字符串数组
 */
inline std::vector<std::string> sf_split_string(std::string str,
                                                const std::string &split_str);

#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCUnusedGlobalDeclarationInspection"

/**
 * 写文件
 * @param file_name 文件名称
 * @param data 数据
 * @param append 是否追加
 * @return 是否写入成功
 */
bool sf_write_file(const std::string &file_name, const byte_array &data,
                   bool append);

#pragma clang diagnostic pop

#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCUnusedGlobalDeclarationInspection"

/**
 * 读文件
 * @param file_name 文件名称
 * @param data 数据
 * @return 是否读取成功
 */
bool sf_read_file(const std::string &file_name, byte_array &data);

#pragma clang diagnostic pop

/**
 * 字符串替换
 * @param str 字符串
 * @param from 要替换的字符串
 * @param to 替换后的新字符串
 */
void sf_string_replace(std::string &str, const std::string &from,
                       const std::string &to);

/**
 * 判断字符串是否是由指定前缀开始的
 * @param str 字符串
 * @param prefix 前缀
 * @return 字符串是否是由指定前缀开始的
 */
bool sf_string_start_with(const std::string &str, const std::string &prefix);

/**
 * 判断字符串是否是由指定后缀结束
 * @param str 字符串
 * @param suffix 后缀
 * @return 字符串是否是由指定后缀结束
 */
bool sf_string_end_with(const std::string &str, const std::string &suffix);

/**
 * 生成时间字符串
 * @return 时间字符串
 */
std::string sf_make_time_str(const std::chrono::system_clock::time_point &tp =
                                 std::chrono::system_clock::now() +
                                 std::chrono::hours(8));

/**
 * long double 转为字符串
 * @param num 数字
 * @return 字符串
 */
std::string sf_long_double_to_string(const long double &num);

/**
 * char容器转换为十六进制字符串
 * @tparam T 容器
 * @param data 数据
 * @return 十六进制字符串
 */
template <typename T>
std::string sf_char_container_to_hex_string(const T &data);

/**
 * 16进制字符串转为char容器
 * @tparam T 容器
 * @param str 字符串
 * @param data 用于返回的数据
 */
template <typename T>
void sf_hex_string_to_char_container(const std::string &str, T &data);

/**
 * @brief 创建实例的类
 * 从这个类继承，就可以获得make_instance接口
 * @tparam T 子类
 * @tparam sf_empty_class 父类
 */
template <typename T, typename Base = sf_empty_class>
struct sf_make_instance_t : public Base {
   private:
    sf_make_instance_t() = default;

   public:
    /**
     * @brief 创建对象
     *
     * @tparam Args 参数类型
     * @param args 参数
     * @return std::shared_ptr<T> 创建的对象
     */
    template <typename... Args>
    static std::shared_ptr<T> make_instance(Args &&... args) {
        return std::shared_ptr<T>(new T(std::forward<Args>(args)...));
    }

    using Base::Base;

    friend T;
};

std::string sf_safe_path(std::string danger_path);

}    // namespace skyfire