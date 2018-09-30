
#pragma once

#include <string>
#include "sf_serialize_binary.hpp"

namespace skyfire
{
    struct sf_http_request_line {
        std::string method;
        std::string url;
        std::string http_version;
    };

    SF_MAKE_SERIALIZABLE_BINARY(sf_http_request_line, method, url, http_version)
}