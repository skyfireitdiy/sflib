#pragma once

#include "sf_http_request.h"
#include "sf_http_response.h"

namespace skyfire {
    /**
     * @brief http中间件接口
     * 
     */
    class http_middleware {
        public:
            virtual bool before(const http_request& http_request, http_response &res) = 0;
            virtual bool after(const http_request& http_request, http_response &res) = 0;
    };
}