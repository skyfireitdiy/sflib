
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

#include <string>
#include <openssl/evp.h>
#include <openssl/bio.h>
#include <openssl/buffer.h>
#include <openssl/sha.h>
#include <zlib.h>
#include <zconf.h>
#include <fstream>
#include <cstring>
#include <sys/stat.h>
#include <sys/types.h>

#include "sf_type.hpp"

namespace skyfire
{
    /**
     * 字符串去除首尾空白
     * @param str 字符串
     * @return 处理后的字符串
     */
    inline std::string sf_string_trim(const std::string& str);
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
    inline bool sf_equal_nocase_string(const std::string& str1,const std::string& str2);

    /**
     * base64编码
     * @param data 数据
     * @return 编码后的base64字符串
     */
    inline std::string sf_base64_encode(const byte_array &data);

    /**
     * base64解码
     * @param data base64数据
     * @return 解码后的数据
     */
    inline byte_array sf_base64_decode(const std::string& data);

    /**
     * sha1编码
     * @param data 数据
     * @return sha1编码后的数据
     */
    inline byte_array sf_sha1_encode(const byte_array &data);

    /**
     * 分割字符串
     * @param str 原始字符串
     * @param split_str 分隔符
     * @return 分割后的字符串数组
     */
    inline std::vector<std::string> sf_split_string(std::string str, const std::string& split_str);

    /**
     * deflate压缩
     * @param input_buffer 缓冲区
     * @return 压缩后的字节数组
     */
    inline byte_array sf_deflate_compress(const byte_array &input_buffer);

    /**
     * 判断路径是否是目录
     * @param path 路径
     * @return 是否是目录
     */
    inline bool sf_is_dir(const std::string& path);

    /**
     * 目录拼接
     * @tparam Str 字符串类型
     * @param str1 当前目录
     * @param str2 后续目录
     * @return 拼接后的目录
     */
    template <typename ... Str>
    std::string sf_path_join(std::string str1, Str... str2);

    /**
     * 目录拼接
     * @param path1 当前目录
     * @param path2 后续目录
     * @return 拼接后的目录
     */
    inline std::string sf_path_join(std::string path1, const std::string path2);

    /**
     * 获取文件大小
     * @param filename 文件名
     * @return 文件大小
     */
    long long sf_get_file_size(const std::string& filename);

    /**
     * 写文件
     * @param file_name 文件名称
     * @param data 数据
     * @param append 是否追加
     * @return 是否写入成功
     */
    bool sf_write_file(const std::string& file_name, const byte_array& data, bool append);

    /**
     * 读文件
     * @param file_name 文件名称
     * @param data 数据
     * @return 是否读取成功
     */
    bool sf_read_file(const std::string& file_name, byte_array& data);

    /**
     * 字符串替换
     * @param str 字符串
     * @param from 要替换的字符串
     * @param to 替换后的新字符串
     */
    void sf_string_replace(std::string& str,const std::string &from, const std::string &to);
}