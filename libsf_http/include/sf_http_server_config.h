#pragma once

#include <string>
#include "sf_serialize.h"

namespace skyfire {

    struct sf_http_server_config {
        std::string host;
        unsigned short port;
        size_t thread_count;
        int request_timeout;
    };

    SF_MAKE_SERIALIZABLE(sf_http_server_config, host, port, thread_count, request_timeout);
}