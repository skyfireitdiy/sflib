#pragma once

#include <string>
#include "sf_serialize.h"

namespace skyfire {

    struct sf_httpserver_config {
        std::string host;
        unsigned short port;
        size_t thread_count;
        std::string static_dir;
    };

    SF_MAKE_SERIALIZABLE(sf_httpserver_config, host, port, thread_count, static_dir);
}