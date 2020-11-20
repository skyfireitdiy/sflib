#pragma once

#include "sf_http_header.h"

namespace skyfire
{

/**
 * @brief http响应头
 * 
 */

class http_server_res_header : public http_header
{
private:
    int                                                 status__       = 200;
    std::string                                         http_version__ = "HTTP/1.1";
    std::string                                         status_desc__  = "OK";
    std::unordered_map<std::string, http_cookie_item_t> res_cookies__;

public:
    /**
     * @brief 添加响应cookie
     * 
     * @param cookies 响应cookie
     */
    void add_cookies(const http_cookie_item_t& cookies);

    /**
     * @brief 添加响应cookie
     * 
     * @param key 键
     * @param value 值
     */
    void add_cookies(const std::string& key, const std::string& value);

    /**
     * @brief 清空响应cookie
     * 
     * @param clear_session 是否清理session
     */
    void clear_cookies(bool clear_session = false);

    /**
     * @brief 删除响应cookie
     * 
     * @param key 键
     */
    void remove_cookies(const std::string& key);

    /**
     * @brief 获取响应cookie
     * 
     * @return std::unordered_map<std::string, http_cookie_item_t> 响应cookie
     */
    std::unordered_map<std::string, http_cookie_item_t> cookies() const;

    /**
     * 生成符合http标准的字符串格式的头文本，用于发送/查看，注意：cookie不包含在头列表中，但是生成字符串时，会将cookie编入
     * @return 生成的头文本
     */
    std::string to_string() const;

    /**
     * 设置http状态码
     * @param status
     */
    void set_status(int status);
    /**
     * 设置http版本（当前只支持HTTP/1.1，默认也是此值）
     * @param http_version
     */
    void set_http_version(const std::string& http_version);
    /**
     * 设置状态码描述信息，一般无需特意设置，在设置status的时候会自动设置
     * @param desc 描述信息
     */
    void set_status_desc(const std::string& desc);

    /**
     * @brief 获取状态码
     * 
     * @return int 状态码
     */
    int status() const;

    /**
     * @brief 获取状态描述
     * 
     * @return std::string 状态描述 
     */
    std::string status_desc() const;

    /**
     * @brief 获取http版本字符串
     * 
     * @return std::string http版本字符串
     */
    std::string http_version() const;
};
}