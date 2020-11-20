
/**
* @version 1.0.0
* @author skyfire
* @file sf_http_server_response.h
*/
#pragma once
#pragma clang diagnostic push
#pragma ide diagnostic   ignored "OCUnusedGlobalDeclarationInspection"

#include "sf_http_server_request.h"
#include "sf_http_server_res_header.h"
#include "sf_http_utils.h"
#include "sf_json.h"
#include "sf_stdc++.h"

namespace skyfire
{
class http_server_base;

/**
 * @brief  HTTP响应
 */
class http_server_response final
{

private:
    http_server_res_header             header__;
    byte_array                         body__;
    http_data_type                     type__ = { http_data_type ::normal };
    http_file_info_t                   file_info__;
    std::vector<http_multipart_info_t> multipart_info_vec__;

    const http_server_request req__;

public:
    /**
     * @brief 构造函数，生成一个response
     * 
     * @param req 关联的req
     */
    http_server_response(const http_server_request& req);

    /**
     * @brief 获取请求对象
     * 
     * @return const http_server_request 请求对象
     */
    const http_server_request get_req() const;

    /**
     * 设置http状态码
     * @param status
     */
    void
    set_status(int status);
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
     * 设置http响应头
     * @param header 响应头
     */
    void set_header(const http_server_res_header& header);

    /**
     * @brief 添加http头
     *
     * @param key 键
     * @param value 值
     */
    void add_header(const std::string& key, const std::string& value);

    /**
     * 设置http响应（会设置类型为normal）
     * @param body 响应体
     */
    void set_body(const byte_array& body);

    /**
     * 设置json响应（会设置content-type）
     * @param json json数据
     */
    void set_json(const json& json);

    /**
     * 设置响应文件（会设置类型为file）
     * @param file_info 文件信息
     */
    void set_file(const http_file_info_t& file_info);

    /**
     * 设置响应文件（默认为整个文件）
     * @param filename 文件名
     */
    void set_file(const std::string& filename);

    /**
     * 设置分块响应（会设置类型为multipart）
     * @param multipart_info_vec 分块信息
     */
    void set_multipart(const std::vector<http_multipart_info_t>& multipart_info_vec);
    /**
     * 添加cookie
     * @param cookie_data cookie信息
     */
    void add_cookie(const http_cookie_item_t& cookie_data);
    /**
     * @brief 添加cookie
     *
     * @param key 键
     * @param value 值
     */
    void add_cookie(const std::string& key, const std::string& value);
    /**
     * @brief 清空cookie
     *
     * @param clear_session 是否清空session
     */
    void clear_cookie(bool clear_session = false);
    /**
     * @brief Set the text object设置纯文本body
     *
     * @param text 文本
     */
    void set_text(const std::string& text);
    /**
     * @brief 设置html body
     *
     * @param html html
     */
    void set_html(const std::string& html);
    /**
     * @brief 设置返回content-type
     *
     * @param content_type content-type
     */
    void set_content_type(const std::string& content_type);

    /**
     * 移除cookie
     * @param key cookie名称
     */
    void remove_cookie(const std::string& key);

    /**
     * 获取cookies
     * @return cookies
     */
    std::unordered_map<std::string, http_cookie_item_t> cookies() const;

    /**
     * 获取响应类型
     * @return 响应类型
     */
    http_data_type type() const;

    /**
     * 获取响应文件
     * @return 响应文件信息
     */
    http_file_info_t file() const;

    /**
     * 获取分块响应信息
     * @return 分块响应信息
     */
    std::vector<http_multipart_info_t> multipart() const;

    /**
     * 获取响应长度
     * @return 响应长度
     */
    unsigned long long length() const;

    /**
     * 获取响应头
     * @return 响应头
     */
    http_header& header();

    /**
     * 转换头部为供发送的字节数组
     * @return 字节数组
     */
    byte_array to_header_package() const;

    /**
     * 转换为供发送的字节数组（在类型为normal时有用）
     * @return 字节数组
     */
    byte_array to_package() const;

    /**
     * 重定向
     * @param new_location 新地址
     * @param code 跳转码
     */
    void redirect(const std::string& new_location, int code = 302);

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

    friend http_server_base;
};
} // namespace skyfire
#pragma clang diagnostic pop