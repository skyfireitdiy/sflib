#pragma once

#include "sf_http_middleware.h"
#include "sf_utils.h"

namespace skyfire {
    class sf_http_middleware_logger: public sf_make_instance_t<sf_http_middleware_logger, sf_http_middleware>
    {
        bool before(const sf_http_request& http_request, sf_http_response& res) override;
        bool after(const sf_http_request& http_request, sf_http_response& res) override;
    };
}