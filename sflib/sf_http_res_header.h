#pragma once

#include "sf_http_header.h"

namespace skyfire {
class sf_http_res_header : public sf_http_header {
private:
    std::unordered_map<std::string, sf_http_cookie_item_t> res_cookies__;

public:
    /**
     * @brief 添加响应cookie
     * 
     * @param cookies 响应cookie
     */
    void add_cookies(const sf_http_cookie_item_t& cookies);

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
     * @return std::unordered_map<std::string, sf_http_cookie_item_t> 响应cookie
     */
    std::unordered_map<std::string, sf_http_cookie_item_t> res_cookies() const;
        
    /**
     * 生成符合http标准的字符串格式的头文本，用于发送/查看，注意：cookie不包含在头列表中，但是生成字符串时，会将cookie编入
     * @return 生成的头文本
     */
    std::string to_string() const;
};
}