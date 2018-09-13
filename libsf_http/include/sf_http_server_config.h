#pragma once

#include <string>
#include "sf_serialize_binary.hpp"

namespace skyfire {

    struct sf_http_server_config {
        std::string host;
        unsigned short port;
        size_t thread_count;
        int request_timeout;
        std::string tmp_file_path;
    };
}