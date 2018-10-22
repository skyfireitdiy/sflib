
/**
* @version 1.0.0
* @author skyfire
* @mail skyfireitdiy@hotmail.com
* @see http://github.com/skyfireitdiy/sflib
* @file sf_http_utils.h

* sflib第一版本发布
* 版本号1.0.0
* 发布日期：2018-10-22
*/

#pragma once

#include <string>
#include <ctime>
#include <map>
#include <memory>
#include <chrono>

#include "sf_tcp_utils.h"
#include "sf_type.h"
#include "sf_http_request_line.h"
#include "sf_serialize_binary.hpp"



namespace skyfire
{
    class sf_http_multipart;

    /**
     * @brief  http头原始类型
     */
    using sf_http_header_t = std::map<std::string,std::string>;

    namespace {
        /**
         * @brief  websocket追加的uuid
         */
        std::string websocket_sha1_append_str = "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";
    }

    /**
     * @brief  请求上下文
     */
    struct sf_request_context_t
    {
        bool new_req = false;   // 是否是新情求
        byte_array buffer;       // buffer
    };

    SF_MAKE_SERIALIZABLE_BINARY(sf_request_context_t,new_req,buffer)


    /**
     *  @brief websocket上下文
     */
    struct sf_websocket_context_t
    {
        std::string url;            // url
        SOCKET sock;                // socket
        byte_array buffer;          // 原始buffer（未解析）
        byte_array data_buffer;     // 解析出来的buffer
    };

    SF_MAKE_SERIALIZABLE_BINARY(sf_websocket_context_t, url, sock, buffer, data_buffer)

    /**
     *  @brief 分块请求上下文
     */
    struct sf_multipart_data_context_t{
        SOCKET sock;                                    // socket
        std::string boundary_str;                       // 分解字符串
        sf_http_header_t header;                        // 头
        sf_http_request_line request_line;              // 请求行
        std::vector<sf_http_multipart> multipart;       // 分块信息
    };

    /**
     * @brief  cookie生存期类型
     */
    enum class cookie_life_type
    {
        session = 0,                        // session生存期
        time_point = 1                      // 时间点生存期
    };

    /**
     *  @brief cookie
     */
    struct sf_http_cookie_t{
        std::string key;                                                                            // 键
        std::string value;                                                                          // 值
        cookie_life_type life_type = cookie_life_type ::session;                                    // 生存期类型
        std::chrono::system_clock::time_point time_point = std::chrono::system_clock::now();        // 生存期
        std::string path = "/";                                                                     // 路径
        bool secure = true;                                                                        // 安全性
        bool http_only = true;                                                                     // http only
    };

    /**
     * 数字转为16进制字符，如10 转为a
     * @param x 数字
     * @return 字符
     */
    inline unsigned char sf_to_hex(unsigned char x);

    /**
     * 16进制字符转为数字，如a转为10
     * @param x 字符
     * @return 数字
     */
    inline unsigned char sf_from_hex(unsigned char x);

    /**
     * url编码
     * @param str 字符串
     * @return 编码后的字符串
     */
    inline std::string sf_url_encode(const std::string &str);

    /**
     * url解码
     * @param str 字符串
     * @return 解码后的字符串
     */
    inline std::string sf_url_decode(const std::string &str);

    /**
     * 解析表单参数
     * @param param_str 要解析的参数字符串
     * @return 解析出来的参数
     */
    std::map<std::string, std::string> sf_parse_param(std::string param_str) ;

    /**
     * 解析url
     * @param raw_url 原始url
     * @param url url
     * @param param 参数
     * @param frame 锚点信息
     */
    inline void sf_parse_url(const std::string &raw_url, std::string &url, std::map<std::string,std::string>& param,
                             std::string frame);


    /**
     * 生成http时间字符串
     * @param tp 时间点
     * @return http时间字符串
     */
    inline std::string sf_make_http_time_str(const std::chrono::system_clock::time_point& tp = std::chrono::system_clock::now());

    /**
     * 转为http头格式格式
     * @param key 原始字符串
     * @return http头格式字符串
     */
    inline std::string sf_to_header_key_format(std::string key);

    /**
     * 读取文件
     * @param filename 文件名
     * @param max_size 最大长度
     * @return 读取结果
     */
    inline byte_array read_file(const std::string& filename, size_t max_size = 4096);

}