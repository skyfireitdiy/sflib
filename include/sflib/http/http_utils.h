
#pragma once
#include "sflib/core/type.h"
#include "sflib/http/http_request_line.h"
#include "sflib/http/http_server_req_multipart.h"
#include "sflib/json/json.h"
#include "sflib/tcp/tcp_utils.h"

#include <openssl/bio.h>
#include <openssl/buffer.h>
#include <openssl/evp.h>
#include <openssl/sha.h>
#include <zconf.h>
#include <zlib.h>

namespace skyfire
{
class http_server_req_multipart;
using http_param_t                         = std::unordered_map<std::string, std::string>;
constexpr char websocket_sha1_append_str[] = "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";
struct request_context_t
{
    bool       new_req = false; // 是否是新情求
    byte_array buffer;          // buffer
};
SF_JSONIFY(request_context_t, new_req, buffer)
struct websocket_context_t final
{
    std::string url;         // url
    SOCKET      sock;        // socket
    byte_array  buffer;      // 原始buffer（未解析）
    byte_array  data_buffer; // 解析出来的buffer
};
SF_JSONIFY(websocket_context_t, url, sock, buffer, data_buffer)
struct http_server_req_multipart_data_context_t final
{
    SOCKET                                 sock;         // socket
    std::string                            boundary_str; // 分解字符串
    http_header_t                          header;       // 头
    http_request_line                      request_line; // 请求行
    std::vector<http_server_req_multipart> multipart;    // 分块信息
};

enum class http_data_type
{
    normal,    // 普通类型
    file,      // 文件类型
    multipart, // multipart类型
    stream
};
struct http_file_info_t final
{
    std::string       filename;
    long long         begin;
    long long         end;
    long unsigned int file_size;
};
struct http_multipart_info_t final
{
    enum class multipart_info_type
    {
        form, // 表单
        file  // 文件
    };
    struct form_info_t final
    {
        http_header_t header; // 响应头
        byte_array    body;   // 响应体
    };
    multipart_info_type
        type; // multipart子类型，当响应类型为multipart时有效
    form_info_t
        form_info; // 表单信息，当type为multipart_info_type::form时有效
    http_file_info_t
        file_info; // 响应文件信息，当type为multipart_info_type::file时有效
};
//////////////////////////////////////////////////////////////////////////////
unsigned char to_hex(unsigned char x);
unsigned char from_hex(unsigned char x);
std::string   url_encode(const std::string& str,
                         bool               decode_plus = true);
std::string   url_decode(const std::string& str,
                         bool               decode_plus = true);
http_param_t  parse_param(std::string param_str);
void          parse_server_req_url(const std::string& raw_url, std::string& url,
                                   http_param_t& param, std::string& frame);
void          parse_client_req_url(std::string raw_url, std::string& agreement, std::string& host, short& port, std::string& resource);
std::string   make_http_time_str(
      const std::chrono::system_clock::time_point& tp = std::chrono::system_clock::now() + std::chrono::hours(8));
std::string to_header_key_format(std::string key);
byte_array  read_file(const std::string& filename,
                      size_t             max_size = 4096);
std::string base64_encode(const byte_array& data);
byte_array  base64_decode(const std::string& data);
byte_array  sha1_encode(const byte_array& data);
byte_array  deflate_compress(const byte_array& input_buffer);
} // namespace skyfire
