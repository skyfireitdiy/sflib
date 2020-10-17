#pragma once

#include "sf_http_middleware.h"
#include "sf_utils.h"

namespace skyfire {
    class http_middleware_logger: public make_instance_t<http_middleware_logger, http_middleware>
    {
        bool before(const http_request& http_request, http_response& res) override;
        bool after(const http_request& http_request, http_response& res) override;
    };
}