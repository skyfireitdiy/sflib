
/**
* @version 1.0.0
* @author skyfire
* @mail skyfireitdiy@hotmail.com
* @see http://github.com/skyfireitdiy/sflib
* @file sf_http_response.h

* sflib第一版本发布
* 版本号1.0.0
* 发布日期：2018-10-22
*/
#pragma once
#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCUnusedGlobalDeclarationInspection"

#include "core/sf_stdc++.h"
#include "sf_http_header.h"
#include "sf_http_utils.h"
#include "tools/sf_json.h"

namespace skyfire {
class sf_http_base_server;

/**
 * @brief  HTTP响应
 */
class sf_http_response final {
   public:
    /**
     * @brief  响应类型
     */
    enum class response_type {
        normal,      // 普通类型
        file,        // 文件类型
        multipart    // multipart类型
    };

    /**
     * @brief
     * 响应文件信息（当响应类型为file或者响应类型为multipart，子类型为file时，使用改数据结构声明文件）
     */
    struct response_file_info_t final {
        std::string filename;
        long long begin;
        long long end;
    };

    /**
     * @brief  multipart类型信息
     */
    struct multipart_info_t final {
        /**
         *  @brief multipart子类型
         */
        enum class multipart_info_type {
            form,    // 表单
            file     // 文件
        };
        /**
         *  @brief 表单信息，当type为multipart_info_type::form时有效
         */
        struct form_info_t final {
            sf_http_header_t header;    // 响应头
            byte_array body;            // 响应体
        };

        multipart_info_type
            type;    // multipart子类型，当响应类型为multipart时有效
        form_info_t
            form_info;    // 表单信息，当type为multipart_info_type::form时有效
        response_file_info_t
            file_info;    // 响应文件信息，当type为multipart_info_type::file时有效
    };

   private:
    int status__ = 200;
    std::string http_version__ = "HTTP/1.1";
    std::string status_desc__ = "OK";
    sf_http_header header__;
    byte_array body__;
    response_type type__ = {response_type ::normal};
    std::unordered_map<std::string, sf_http_cookie_t> cookies__;
    response_file_info_t file_info__;
    std::vector<multipart_info_t> multipart_info_vec__;

   public:
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
     * 设置http响应头
     * @param header 响应头
     */
    void set_header(const sf_http_header& header);

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
    void set_json(const sf_json& json);

    /**
     * 设置响应文件（会设置类型为file）
     * @param file_info 文件信息
     */
    void set_file(const response_file_info_t& file_info);

    /**
     * 设置响应文件（默认为整个文件）
     * @param filename 文件名
     */
    void set_file(const std::string& filename);

    /**
     * 设置分块响应（会设置类型为multipart）
     * @param multipart_info_vec 分块信息
     */
    void set_multipart(const std::vector<multipart_info_t>& multipart_info_vec);
    /**
     * 添加cookie
     * @param cookie_data cookie信息
     */
    void add_cookie(const sf_http_cookie_t& cookie_data);
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
    std::unordered_map<std::string, sf_http_cookie_t> cookies() const;

    /**
     * 获取响应类型
     * @return 响应类型
     */
    response_type type() const;

    /**
     * 获取响应文件
     * @return 响应文件信息
     */
    response_file_info_t file() const;

    /**
     * 获取分块响应信息
     * @return 分块响应信息
     */
    std::vector<multipart_info_t> multipart() const;

    /**
     * 获取响应长度
     * @return 响应长度
     */
    unsigned long long length() const;

    /**
     * 获取响应头
     * @return 响应头
     */
    sf_http_header& header();

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

    friend sf_http_base_server;
};
}    // namespace skyfire
#pragma clang diagnostic pop