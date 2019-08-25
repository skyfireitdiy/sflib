
/**
* @version 1.0.0
* @author skyfire
* @mail skyfireitdiy@hotmail.com
* @see http://github.com/skyfireitdiy/sflib
* @file sf_type.h

* sflib第一版本发布
* 版本号1.0.0
* 发布日期：2018-10-22
*/

/*
 * 常用类型声明
 */

#pragma once

#include <fstream>
#include <string>
#include <vector>

#ifndef _WIN32
#define SOCKET int
#endif

#define SF_GET_OFFSET(obj, member) \
    (reinterpret_cast<size_t>(&(reinterpret_cast<obj *>(0)->member)))

#include <cstring>
#include <iostream>

namespace skyfire {
/**
 *  @brief 字节数组类型
 */
using byte_array = std::vector<char>;

/**
 * 字节数组转字符串
 * @param data 字节数组
 * @return 字符串
 */
inline std::string to_string(const byte_array &data);

/**
 * 合并字节数组
 * @param b1 字节数组1
 * @param b2 字节数组2
 * @return 字节数组合并
 */
inline byte_array operator+(byte_array b1, const byte_array &b2);

/**
 * 追加字节数组
 * @param b1 字节数组1
 * @param b2 字节数组2
 * @return 新的字节数组
 */
inline byte_array &operator+=(byte_array &b1, const byte_array &b2);

/**
 * pod类型转换为字节数组
 * @tparam T 类型
 * @param t 数据
 * @return 字节数组
 */
template <typename T>
inline byte_array to_byte_array(const T &t);

/**
 * 字符串转换为字节数组
 * @param str 字符串
 * @return 字节数组
 */
inline byte_array to_byte_array(const std::string &str);

/**
 * 保存字节数组到文件
 * @param data 数据
 * @param filename 文件名称
 * @param app 是否是追加
 * @return 是否dump成功
 */
inline bool dump_byte_array(const byte_array &data, const std::string &filename,
                            bool app = false);
}    // namespace skyfire
