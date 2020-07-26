#pragma once

#include "sf_http_request.h"
#include "sf_http_response.h"

namespace skyfire {
    class sf_http_middleware {
        public:
            virtual bool before(const sf_http_request& http_request, sf_http_response &res) = 0;
            virtual bool after(const sf_http_request& http_request, sf_http_response &res) = 0;
    };
}