
/**
* @version 1.0.0
* @author skyfire
* @file sf_http_request_line.h
*/

#pragma once

#include "sf_json.hpp"

namespace skyfire {
/**
 *  @brief 请求行数据
 */
struct http_request_line final {
    std::string method; // 请求方法
    std::string url; // 请求url
    std::string http_version = "HTTP/1.1"; // http版本，目前支持持HTTP/1.1
};

SF_JSONIFY(http_request_line, method, url, http_version)
} // namespace skyfire