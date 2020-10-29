#pragma once

#include "sf_http_header.h"
#include "sf_http_request_line.h"

namespace skyfire {
class http_client_req_header : public http_header {
private:
    std::unordered_map<std::string, std::string> cookie__;
    http_request_line request_line__;

public:
    /**
     * @brief 转为字符串
     * 
     * @return std::string 转换结果
     */
    std::string to_string() const;

    /**
     * @brief 增加cookies
     * 
     * @param key 键
     * @param value 值 
     */
    void add_cookies(const std::string& key, const std::string& value);

    /**
     * @brief 设置请求行  
     * 
     * @param request_line 请求行
     */
    void set_request_line(const http_request_line& request_line);
};
}