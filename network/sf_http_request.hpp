
/**
* @version 1.0.0
* @author skyfire
* @mail skyfireitdiy@hotmail.com
* @see http://github.com/skyfireitdiy/sflib
* @file sf_http_request.hpp

* sflib第一版本发布
* 版本号1.0.0
* 发布日期：2018-10-22
*/

#pragma once
#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCUnusedGlobalDeclarationInspection"

#include "sf_http_request.h"
#include "sf_http_request_line.h"
#include "tools/sf_utils.hpp"
// ReSharper disable once CppUnusedIncludeDirective
#include "sf_http_utils.hpp"
#include "tools/sf_logger.hpp"
#include "tools/sf_random.hpp"

namespace skyfire {

inline bool sf_http_request::split_request(
    const byte_array &raw, std::string &request_line,
    std::vector<std::string> &header_lines, byte_array &body) {
    auto raw_string = to_string(raw);
    const auto pos = raw_string.find("\r\n\r\n");
    if (pos == std::string::npos) return false;
    body = byte_array(raw.begin() + pos + 4, raw.end());
    std::istringstream si(
        std::string(raw_string.begin(), raw_string.begin() + pos));
    getline(si, request_line);
    std::string tmp_str;
    header_lines.clear();
    while (!si.eof()) {
        getline(si, tmp_str);
        header_lines.push_back(tmp_str);
    }
    return true;
}

inline byte_array sf_http_request::body() const { return body__; }

inline sf_http_header sf_http_request::header() const { return header__; }

inline sf_http_request_line sf_http_request::request_line() const {
    return request_line__;
}

inline bool sf_http_request::is_valid() const { return valid__; }

inline sf_http_request::sf_http_request(byte_array raw, SOCKET sock)
    : raw__(std::move(raw)), sock__(sock) {
    valid__ = parse_request__();
}

inline bool sf_http_request::parse_request__() {
    std::string request_line;
    std::vector<std::string> header_lines;
    if (!split_request(raw__, request_line, header_lines, body__)) {
        sf_debug("split request error");
        return false;
    }
    if (!parse_request_line(request_line, request_line__)) {
        sf_debug("parse request line error");
        return false;
    }
    if (!parse_header(header_lines, header__)) {
        sf_debug("parse header error");
        return false;
    }

    parse_cookies(header__, cookies__);

    auto content_len = header__.header_value("Content-Length", "0");

    auto content_type = header__.header_value("Content-Type", "");
    if (!content_type.empty()) {
        auto content_type_list = sf_split_string(content_type, ";");
        for (const auto &p : content_type_list) {
            auto tmp_str = sf_string_trim(p);
            if (tmp_str.find("boundary=") == 0) {
                multipart_data__ = true;
                auto boundary_str_list = sf_split_string(tmp_str, "=");
                if (boundary_str_list.size() != 2) {
                    sf_error("boundary str size error");
                    error__ = true;
                    return false;
                }
                multipart_data_context__.request_line = request_line__;
                if (boundary_str_list[1].size() <= 2) {
                    sf_error("boundary is too short");
                    error__ = true;
                    return false;
                }
                multipart_data_context__.boundary_str = {
                    boundary_str_list[1].begin() + 2,
                    boundary_str_list[1].end()};
                sf_debug("boundary_str", multipart_data_context__.boundary_str);
                multipart_data_context__.header = header__.header();
                return true;
            }
        }
    }
    char *pos;
    if (std::strtoll(content_len.c_str(), &pos, 10) != body__.size()) {
        sf_debug("body size error", content_len.c_str(), body__.size());
        return false;
    }

    return true;
}

inline bool sf_http_request::parse_header(
    const std::vector<std::string> header_lines, sf_http_header &header) {
    for (auto &line : header_lines) {
        const auto pos = line.find(':');
        if (pos == std::string::npos) return false;
        const std::string key(line.begin(), line.begin() + pos);
        std::string value(line.begin() + pos + 1, line.end());
        value = sf_string_trim(value);
        header.set_header(key, value);
    }
    return true;
}

inline bool sf_http_request::parse_request_line(
    const std::string &request_line, sf_http_request_line &request_line_para) {
    std::istringstream si(request_line);
    if (!(si >> request_line_para.method)) return false;
    if (!(si >> request_line_para.url)) return false;
    return !!(si >> request_line_para.http_version);
}

inline bool sf_http_request::is_multipart_data() const {
    return multipart_data__;
}

inline sf_multipart_data_context_t sf_http_request::multipart_data_context()
    const {
    return multipart_data_context__;
}

inline sf_http_request::sf_http_request(
    sf_multipart_data_context_t multipart_data, SOCKET sock) {
    valid__ = true;
    request_line__ = multipart_data.request_line;
    header__.set_header(multipart_data.header);
    multipart_data__ = true;
    multipart_data_context__ = multipart_data;
    sock__ = sock;
    parse_cookies(header__, cookies__);
}

inline void sf_http_request::parse_cookies(
    const sf_http_header &header_data,
    std::unordered_map<std::string, std::string> &cookies) {
    cookies.clear();
    if (header_data.has_key("Cookie")) {
        const auto cookie_str = header_data.header_value("Cookie");
        auto str_list = sf_split_string(cookie_str, ";");
        for (auto &p : str_list) {
            p = sf_string_trim(p);
        }
        for (auto &p : str_list) {
            auto tmp_list = sf_split_string(p, "=");
            if (tmp_list.size() != 2) {
                continue;
            }
            cookies[tmp_list[0]] = tmp_list[1];
        }
    }
    if (cookies.count(sf_session_id_key) == 0 ||
        cookies[sf_session_id_key].empty()) {
        auto rd = sf_random::instance();
        cookies[sf_session_id_key] = rd->uuid_str();
    }
}

inline std::unordered_map<std::string, std::string> sf_http_request::cookies()
    const {
    return cookies__;
}

inline std::string sf_http_request::session_id() const {
    auto ck = cookies();
    if (cookies__.count(sf_session_id_key) == 0) {
        return "";
    }
    return ck[sf_session_id_key];
}

inline bool sf_http_request::is_error() const { return error__; }

inline std::string sf_http_request::url() const { return request_line__.url; }

inline std::string sf_http_request::base_url() const {
    std::string ret;
    sf_http_param_t params;
    std::string frame;
    sf_parse_url(request_line__.url, ret, params, frame);
    return ret;
}

inline sf_addr_info_t sf_http_request::addr() const {
    sf_addr_info_t ret;
    peer_addr(sock__, ret);
    return ret;
}

inline sf_http_param_t sf_http_request::params() const {
    std::string url;
    sf_http_param_t params;
    std::string frame;
    sf_parse_url(request_line__.url, url, params, frame);
    return params;
}

inline sf_http_param_t sf_http_request::body_params() const {
    return sf_parse_param(to_string(body__));
}

inline std::string sf_http_request::frame() const {
    std::string url;
    sf_http_param_t params;
    std::string frame;
    sf_parse_url(request_line__.url, url, params, frame);
    return frame;
}

inline std::string sf_http_request::header(
    const std::string &key, const std::string &default_value) const {
    return header__.header_value(key, default_value);
}

}    // namespace skyfire
#pragma clang diagnostic pop