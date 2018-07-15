
#pragma once

#include <string>

namespace skyfire
{
    struct sf_http_request_line {
        std::string method;
        std::string url;
        std::string http_version;
    };
}