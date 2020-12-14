
/**
* @version 1.0.0
* @author skyfire
* @file sf_utils.h
*/

#pragma once

#include "sf_empty_class.h"
#include "sf_stdc++.h"
#include "sf_type.hpp"
#include <sys/stat.h>
#include <sys/types.h>

namespace skyfire
{
/**
 * 字符串去除首尾空白
 * @param str 字符串
 * @return 处理后的字符串
 */
inline std::string string_trim(const std::string& str);

/**
 * 字符串转小写
 * @param str 字符串
 * @return 处理后的字符串
 */
inline std::string to_lower_string(std::string str);

/**
 * 获取文件后缀
 * @param path 路径
 * @return 后缀
 */
inline std::string get_path_ext(const std::string& path);

/**
 * 不区分大小写比较字符串
 * @param str1 字符串1
 * @param str2 字符串2
 * @return 是否相等
 */
inline bool equal_nocase_string(const std::string& str1,
                                const std::string& str2);

/**
 * 分割字符串
 * @param str 原始字符串
 * @param split_str 分隔符
 * @return 分割后的字符串数组
 */
inline std::vector<std::string> split_string(std::string        str,
                                             const std::string& split_str);


/**
 * 写文件
 * @param file_name 文件名称
 * @param data 数据
 * @param append 是否追加
 * @return 是否写入成功
 */
bool write_file(const std::string& file_name, const byte_array& data,
                bool append);

/**
 * 读文件
 * @param file_name 文件名称
 * @param data 数据
 * @return 是否读取成功
 */
bool read_file(const std::string& file_name, byte_array& data);


/**
 * 字符串替换
 * @param str 字符串
 * @param from 要替换的字符串
 * @param to 替换后的新字符串
 */
void string_replace(std::string& str, const std::string& from,
                    const std::string& to);

/**
 * 判断字符串是否是由指定前缀开始的
 * @param str 字符串
 * @param prefix 前缀
 * @return 字符串是否是由指定前缀开始的
 */
bool string_start_with(const std::string& str, const std::string& prefix);

/**
 * 判断字符串是否是由指定后缀结束
 * @param str 字符串
 * @param suffix 后缀
 * @return 字符串是否是由指定后缀结束
 */
bool string_end_with(const std::string& str, const std::string& suffix);

/**
 * 生成时间字符串
 * @return 时间字符串
 */
std::string make_time_str(const std::chrono::system_clock::time_point& tp  = std::chrono::system_clock::now(),
                          const std::string&                           fmt = "%Y-%m-%d %H:%M:%S");

std::string make_time_str(const std::filesystem::file_time_type& tp, const std::string& fmt = "%Y-%m-%d %H:%M:%S");

/**
 * long double 转为字符串
 * @param num 数字
 * @return 字符串
 */
std::string long_double_to_string(const long double& num);

/**
 * char容器转换为十六进制字符串
 * @tparam T 容器
 * @param data 数据
 * @return 十六进制字符串
 */
template <typename T>
std::string char_container_to_hex_string(const T& data);

/**
 * 16进制字符串转为char容器
 * @tparam T 容器
 * @param str 字符串
 * @param data 用于返回的数据
 */
template <typename T>
void hex_string_to_char_container(const std::string& str, T& data);

/**
 * @brief 转换毫秒为人类可读的字符串
 * 
 * @param time 时间
 * @return std::string 可读字符串
 */
std::string convert_ns_to_readable(long long time);

/**
 * @brief 创建实例的类
 * 从这个类继承，就可以获得make_instance接口
 * @tparam T 子类
 * @tparam empty_class 父类
 */
template <typename T, typename Base = empty_class>
struct make_instance_t : public Base
{
private:
    make_instance_t() = default;

public:
    /**
     * @brief 创建对象
     *
     * @tparam Args 参数类型
     * @param args 参数
     * @return std::shared_ptr<T> 创建的对象
     */
    template <typename... Args>
    static std::shared_ptr<T> make_instance(Args&&... args)
    {
        return std::shared_ptr<T>(new T(std::forward<Args>(args)...));
    }

    using Base::Base;

    friend T;
};

std::string safe_path(std::string danger_path);

} // namespace skyfire