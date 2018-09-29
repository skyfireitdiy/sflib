#pragma once

#include <fstream>
#include <memory>
#include <string>
#include "sf_http_router.hpp"
#include "sf_define.hpp"
#include "sf_http_utils.hpp"
#include "sf_content_type.hpp"
#include "sf_logger.hpp"


namespace skyfire {
    using namespace std::literals;

    inline std::shared_ptr<sf_http_router>
    make_static_router(std::string path, const std::vector<std::string> &methods = {{"*"s}},
                       std::string charset = "utf-8",
                       bool deflate = true,
                       unsigned long long max_file_size = DEFAULT_HTTP_MAX_FILE_SIZE,
                       int priority = DEFAULT_HTTP_STATIC_PRIORITY

    );
}