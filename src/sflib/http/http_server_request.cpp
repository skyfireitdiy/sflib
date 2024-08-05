#include "sflib/http/http_server_request.h"
#include "sflib/http/http_request_line.h"
#include "sflib/http/http_server_req_header.h"
#include "sflib/http/http_session_id_key.h"
#include "sflib/http/http_utils.h"
#include "sflib/tools/random.h"
#include "sflib/tools/utils.h"

namespace skyfire
{
bool http_server_request::split_request(
    const byte_array &raw, std::string &request_line,
    std::vector<std::string> &header_lines, byte_array &body)
{
    auto raw_string = to_string(raw);
    const auto pos = raw_string.find("\r\n\r\n");
    if (pos == std::string::npos)
        return false;
    body = byte_array(raw.begin() + pos + 4, raw.end());
    std::istringstream si(
        std::string(raw_string.begin(), raw_string.begin() + pos));
    getline(si, request_line);
    std::string tmp_str;
    header_lines.clear();
    while (!si.eof())
    {
        getline(si, tmp_str);
        header_lines.push_back(tmp_str);
    }
    return true;
}
byte_array http_server_request::body() const
{
    return body__;
}
http_server_req_header http_server_request::header() const
{
    return header__;
}
http_request_line http_server_request::request_line() const
{
    return request_line__;
}
bool http_server_request::is_valid() const
{
    return valid__;
}
http_server_request::http_server_request(byte_array raw, SOCKET sock)
    : raw__(std::move(raw))
    , sock__(sock)
{
    valid__ = parse_request__();
}
bool http_server_request::parse_request__()
{
    std::string request_line;
    std::vector<std::string> header_lines;
    if (!split_request(raw__, request_line, header_lines, body__))
    {

        return false;
    }
    if (!parse_request_line(request_line, request_line__))
    {

        return false;
    }
    if (!parse_header(header_lines, header__))
    {

        return false;
    }
    parse_cookies(header__, cookies__);
    auto content_len = header__.header_value("Content-Length", "0");
    auto content_type = header__.header_value("Content-Type", "");
    if (!content_type.empty())
    {
        auto content_type_list = split_string(content_type, ";");
        for (const auto &p : content_type_list)
        {
            auto tmp_str = string_trim(p);
            if (tmp_str.find("boundary=") == 0)
            {
                multipart_data__ = true;
                auto boundary_str_list = split_string(tmp_str, "=");
                if (boundary_str_list.size() != 2)
                {

                    error__ = true;
                    return false;
                }
                multipart_data_context__.request_line = request_line__;
                if (boundary_str_list[1].size() <= 2)
                {

                    error__ = true;
                    return false;
                }
                multipart_data_context__.boundary_str = {
                    boundary_str_list[1].begin() + 2,
                    boundary_str_list[1].end()};

                multipart_data_context__.header = header__.header();
                return true;
            }
        }
    }
    char *pos;
    if (std::strtoull(content_len.c_str(), &pos, 10) != body__.size())
    {

        return false;
    }
    return true;
}
bool http_server_request::parse_header(
    const std::vector<std::string> header_lines, http_header &header)
{
    for (auto &line : header_lines)
    {
        const auto pos = line.find(':');
        if (pos == std::string::npos)
            return false;
        const std::string key(line.begin(), line.begin() + pos);
        std::string value(line.begin() + pos + 1, line.end());
        value = string_trim(value);
        header.set_header(key, value);
    }
    return true;
}
bool http_server_request::parse_request_line(
    const std::string &request_line, http_request_line &request_line_para)
{
    std::istringstream si(request_line);
    if (!(si >> request_line_para.method))
        return false;
    if (!(si >> request_line_para.url))
        return false;
    return !!(si >> request_line_para.http_version);
}
bool http_server_request::is_multipart_data() const
{
    return multipart_data__;
}
http_server_req_multipart_data_context_t http_server_request::multipart_data_context()
    const
{
    return multipart_data_context__;
}
http_server_request::http_server_request(
    http_server_req_multipart_data_context_t multipart_data, SOCKET sock)
{
    valid__ = true;
    request_line__ = multipart_data.request_line;
    header__.set_header(multipart_data.header);
    multipart_data__ = true;
    multipart_data_context__ = multipart_data;
    sock__ = sock;
    parse_cookies(header__, cookies__);
}
void http_server_request::parse_cookies(
    const http_header &header_data,
    std::unordered_map<std::string, std::string> &cookies)
{
    cookies.clear();
    if (header_data.has_key("Cookie"))
    {
        const auto cookie_str = header_data.header_value("Cookie");
        auto str_list = split_string(cookie_str, ";");
        for (auto &p : str_list)
        {
            p = string_trim(p);
        }
        for (auto &p : str_list)
        {
            auto tmp_list = split_string(p, "=");
            if (tmp_list.size() != 2)
            {
                continue;
            }
            cookies[tmp_list[0]] = tmp_list[1];
        }
    }
    if (cookies.count(session_id_key) == 0 || cookies[session_id_key].empty())
    {
        auto rd = random::instance();
        cookies[session_id_key] = rd->uuid_str();
    }
}
std::unordered_map<std::string, std::string> http_server_request::cookies()
    const
{
    return cookies__;
}
std::string http_server_request::session_id() const
{
    auto ck = cookies();
    if (cookies__.count(session_id_key) == 0)
    {
        return "";
    }
    return ck[session_id_key];
}
bool http_server_request::is_error() const
{
    return error__;
}
std::string http_server_request::url() const
{
    return request_line__.url;
}
std::string http_server_request::base_url() const
{
    std::string ret;
    http_param_t params;
    std::string frame;
    parse_server_req_url(request_line__.url, ret, params, frame);
    return ret;
}
addr_info_t http_server_request::addr() const
{
    addr_info_t ret;
    peer_addr(sock__, ret);
    return ret;
}
http_param_t http_server_request::params() const
{
    std::string url;
    http_param_t params;
    std::string frame;
    parse_server_req_url(request_line__.url, url, params, frame);
    return params;
}
http_param_t http_server_request::body_params() const
{
    return parse_param(to_string(body__));
}
std::string http_server_request::frame() const
{
    std::string url;
    http_param_t params;
    std::string frame;
    parse_server_req_url(request_line__.url, url, params, frame);
    return frame;
}
std::string http_server_request::header(
    const std::string &key, const std::string &default_value) const
{
    return header__.header_value(key, default_value);
}
void http_server_request::set_body(const byte_array &body)
{
    body__ = body;
}
void http_server_request::set_body(std::shared_ptr<std::istream> ptr)
{
    istream_ptr__ = ptr;
}
void http_server_request::set_request_line(const http_request_line &request_line)
{
    request_line__ = request_line;
}
void http_server_request::set_header(const http_server_req_header &header)
{
    header__ = header;
}
void http_server_request::set_cookies(const std::unordered_map<std::string, std::string> &cookies)
{
    cookies__ = cookies;
}
byte_array http_server_request::gen_req_data() const
{
    // TODO 生成请求数据
    return byte_array();
}
} // namespace skyfire
