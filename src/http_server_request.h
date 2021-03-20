
#pragma once
#include "http_header.h"
#include "http_request_line.h"
#include "http_server_req_header.h"
#include "http_utils.h"
#include "logger.h"
#include "tcp_server.h"
#include "type.h"
#include "utils.h"
namespace skyfire
{
class http_server_request final
{
private:
    const byte_array                             raw__;
    bool                                         valid__ = false;
    bool                                         error__ = false;
    http_request_line                            request_line__;
    http_server_req_header                       header__;
    byte_array                                   body__;
    bool                                         multipart_data__ = false;
    http_server_req_multipart_data_context_t     multipart_data_context__;
    std::unordered_map<std::string, std::string> cookies__;
    std::shared_ptr<std::istream>                istream_ptr__ = nullptr;
    SOCKET                                       sock__;
    bool                                         parse_request__();

public:
    explicit http_server_request(byte_array raw, SOCKET sock);
    void       set_body(const byte_array& body);
    void       set_body(std::shared_ptr<std::istream> ptr);
    void       set_request_line(const http_request_line& request_line);
    void       set_header(const http_server_req_header& header);
    void       set_cookies(const std::unordered_map<std::string, std::string>& cookies);
    byte_array gen_req_data() const;
    explicit http_server_request(http_server_req_multipart_data_context_t multipart_data,
                                 SOCKET                                   sock);
    bool                                         is_valid() const;
    bool                                         is_error() const;
    http_request_line                            request_line() const;
    http_server_req_header                       header() const;
    std::string                                  header(const std::string& key,
                                                        const std::string& default_value = "") const;
    byte_array                                   body() const;
    bool                                         is_multipart_data() const;
    http_server_req_multipart_data_context_t     multipart_data_context() const;
    std::unordered_map<std::string, std::string> cookies() const;
    std::string                                  session_id() const;
    std::string                                  url() const;
    std::string                                  base_url() const;
    addr_info_t                                  addr() const;
    http_param_t                                 params() const;
    http_param_t                                 body_params() const;
    std::string                                  frame() const;
    static bool                                  split_request(const byte_array& raw, std::string& request_line,
                                                               std::vector<std::string>& header_lines,
                                                               byte_array&               body);
    static bool                                  parse_request_line(const std::string& request_line,
                                                                    http_request_line& request_line_para);
    static bool                                  parse_header(std::vector<std::string> header_lines,
                                                              http_header&             header);
    static void                                  parse_cookies(
                                         const http_header&                            header_data,
                                         std::unordered_map<std::string, std::string>& cookies);
};
} // namespace skyfire
