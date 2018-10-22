
/**
* @version 1.0.0
* @author skyfire
* @email skyfireitdiy@hotmail.com
* @see http://github.com/skyfireitdiy/sflib
* @file sf_http_request_line.h

* sflib第一版本发布
* 版本号1.0.0
* 发布日期：2018-10-22
*/


#pragma once

#include <string>
#include "sf_serialize_binary.hpp"

namespace skyfire
{
    /**
     *  @brief 请求行数据
     */
    struct sf_http_request_line {
        std::string method;             // 请求方法
        std::string url;                // 请求url
        std::string http_version;       // http版本，目前支持持HTTP/1.1
    };

    SF_MAKE_SERIALIZABLE_BINARY(sf_http_request_line, method, url, http_version)
}