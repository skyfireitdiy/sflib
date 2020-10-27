#pragma once

#include "sf_http_server_request.h"
#include "sf_http_server_response.h"

namespace skyfire {
    /**
     * @brief http中间件接口
     * 
     */
    class http_middleware {
        public:
            virtual bool before(const http_server_request& http_server_request, http_server_response &res) = 0;
            virtual bool after(const http_server_request& http_server_request, http_server_response &res) = 0;
    };
}