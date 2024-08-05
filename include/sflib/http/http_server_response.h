
#pragma once
#include "sflib/http/http_server_request.h"
#include "sflib/http/http_server_res_header.h"
#include "sflib/http/http_utils.h"
#include "sflib/json/json.h"

namespace skyfire
{
class http_server_base;
class http_server_response final
{
private:
    http_server_res_header             header__;
    byte_array                         body__;
    http_data_type                     type__ = { http_data_type ::normal };
    http_file_info_t                   file_info__;
    std::vector<http_multipart_info_t> multipart_info_vec__;
    const http_server_request          req__;

public:
    http_server_response(const http_server_request& req);
    const http_server_request get_req() const;
    void
                                                        set_status(int status);
    void                                                set_http_version(const std::string& http_version);
    void                                                set_status_desc(const std::string& desc);
    void                                                set_header(const http_server_res_header& header);
    void                                                add_header(const std::string& key, const std::string& value);
    void                                                set_body(const byte_array& body);
    void                                                set_json(const json& json);
    void                                                set_file(const http_file_info_t& file_info);
    void                                                set_file(const std::string& filename);
    void                                                set_multipart(const std::vector<http_multipart_info_t>& multipart_info_vec);
    void                                                add_cookie(const http_cookie_item_t& cookie_data);
    void                                                add_cookie(const std::string& key, const std::string& value);
    void                                                clear_cookie(bool clear_session = false);
    void                                                set_text(const std::string& text);
    void                                                set_html(const std::string& html);
    void                                                set_content_type(const std::string& content_type);
    void                                                remove_cookie(const std::string& key);
    std::unordered_map<std::string, http_cookie_item_t> cookies() const;
    http_data_type                                      type() const;
    http_file_info_t                                    file() const;
    std::vector<http_multipart_info_t>                  multipart() const;
    unsigned long long                                  length() const;
    http_header&                                        header();
    byte_array                                          to_header_package() const;
    byte_array                                          to_package() const;
    void                                                redirect(const std::string& new_location, int code = 302);
    int                                                 status() const;
    std::string                                         status_desc() const;
    std::string                                         http_version() const;
    friend http_server_base;
};
} // namespace skyfire
