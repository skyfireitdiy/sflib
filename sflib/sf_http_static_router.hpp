
/**
* @version 1.0.0
* @author skyfire
* @file sf_http_static_router.hpp

* 
*/

/**
* @author skyfire
* @file sf_http_static_router.hpp
*/

#pragma once

#pragma clang diagnostic push
#pragma ide diagnostic ignored "cert-err34-c"
#pragma ide diagnostic ignored "bugprone-lambda-function-name"

#include "sf_http_static_router.h"

#include "sf_msvc_safe.h"
#include "sf_stdc++.h"
#include "sf_utils.hpp"
#include <utility>

namespace skyfire {
using namespace std::literals;

inline bool static_router::run_route(const http_server_request& req,
    http_server_response& res,
    const std::string& raw_url,
    const std::string& method)
{
    std::string url;
    http_param_t param;
    std::string frame;
    parse_url(raw_url, url, param, frame);
    if (string_start_with(url, "/")) {
        url = url.substr(1);
    }
    auto abs_path = fs::path(static_path__) / url;
    http_header header;
    byte_array body_data;
    if (!fs::exists(abs_path) || fs::is_directory(abs_path)) {
        return false;
    }
    callback__(req, res, url, abs_path);
    return true;
}

inline static_router::static_router(std::string path,
    std::vector<std::string> methods,
    std::string charset, bool deflate,
    int priority)
    : static_path__(std::move(path))
    , methods__(std::move(methods))
    , priority__(priority)
{
    if (!static_path__.empty()) {
        const auto ch = static_path__[static_path__.length() - 1];
        if (ch == '\\' || ch == '/') {
            static_path__.pop_back();
        }
    }

    callback__ = [=](const http_server_request& req, http_server_response& res,
                     const std::string& url, const std::string& abs_path) {
        http_res_header header;
        byte_array body_data;

        header.set_header("Date", make_http_time_str());

        auto _401_res = [&] {
            res.set_status(401);
            header.set_header("Content-Type", "text/html; charset=" + charset);
            body_data = to_byte_array("<p>" + url + " bad request!</p>");
        };
        auto _404_res = [&] {
            res.set_status(404);
            body_data = to_byte_array("<p>" + url + " not found</p>");
            header.set_header("Content-Type", "text/html; charset=" + charset);
        };
        auto _416_res = [&] {
            res.set_status(416);
            body_data = to_byte_array("<p>" + url + " Requested Range Not Satisfiable</p>");
            header.set_header("Content-Type", "text/html; charset=" + charset);
        };

        auto file_size = fs::file_size(abs_path);
        if (file_size == -1) {
            _404_res();
        } else {
            if (req.header().has_key("Range")) {
                debug("Range found");
                auto range_header = req.header().header_value("Range", "");
                auto range_content_list = split_string(range_header, "=");
                if (range_content_list.size() < 2) {
                    _401_res();
                } else {
                    auto range_list = split_string(range_content_list[1], ",");
                    for (auto& p : range_list) {
                        p = string_trim(p);
                    }
                    if (range_list.size() > 1) {
                        std::vector<http_server_response::multipart_info_t>
                            multipart_info_vec;
                        bool error_flag = false;
                        for (auto& range_str : range_list) {
#if defined(LLONG_MAX)
                            long long start = LLONG_MAX;
#elif defined(LONG_LONG_MAX)
                            long long start = LONG_LONG_MAX;
#else
                            #error long long max not define!
#endif
                            long long end = -1;
#ifdef _MSC_VER
                            if (scanf_s(range_str.c_str(), "%lld-%lld", &start,
                                    &end)
                                < 1)
#else
                            if (sscanf(range_str.c_str(), "%lld-%lld", &start,
                                    &end)
                                < 1)
#endif
                            {
                                error_flag = true;
                                _416_res();
                                break;
                            }
                            http_server_response::multipart_info_t tmp_part;
                            tmp_part.type = http_server_response::multipart_info_t::
                                multipart_info_type::file;
                            tmp_part.file_info = http_server_response::response_file_info_t {
                                abs_path, start, end
                            };
                            multipart_info_vec.emplace_back(tmp_part);
                        }
                        if (!error_flag) {
                            res.set_header(header);
                            res.set_multipart(multipart_info_vec);
                            return;
                        }
                    } else {
#if defined(LLONG_MAX)
                        auto start = LLONG_MAX;
#elif defined(LONG_LONG_MAX)
                        auto start = LONG_LONG_MAX;
#else
                        #error long long max not define!
#endif 
                        long long end = -1;
#ifdef _MSC_VER
                        if (sscanf_s(range_list[0].c_str(), "%lld-%lld", &start,
                                &end)
                            < 1)
#else
                        if (sscanf(range_list[0].c_str(), "%lld-%lld", &start,
                                &end)
                            < 1)
#endif // _MSC_VER
                        {
                            _401_res();
                        } else {
                            res.set_header(header);
                            res.set_file(http_server_response::response_file_info_t {
                                abs_path, start, end, file_size });
                            return;
                        }
                    }
                }
            } else {
                debug("big file", abs_path);
                res.set_header(header);
                res.set_file(http_server_response::response_file_info_t { abs_path, 0, -1, file_size });
                return;
            }
        }

        auto accept = req.header().header_value("Accept-Encoding", "");
        auto accept_list = split_string(accept, ",");
        for (auto& p : accept_list)
            p = string_trim(p);

        if (deflate && std::find_if(accept_list.begin(), accept_list.end(), [=](const std::string& str) {
                return equal_nocase_string(
                    str, "deflate");
            }) != accept_list.end()) {
            body_data = deflate_compress(body_data);
            header.set_header("Content-Encoding", "deflate");
        }
        res.set_header(header);
        res.set_body(body_data);
    };
}

inline int static_router::priority() const { return priority__; }

} // namespace skyfire

#pragma clang diagnostic pop