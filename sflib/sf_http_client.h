#pragma once

#include "sf_http_client_request.h"
#include "sf_nocopy.h"

namespace skyfire {

using http_request_result = multi_value<err, http_client_response>;
class http_client : public nocopy<> {
private:
public:
    std::shared_ptr<http_request_result> request(std::shared_ptr<http_client_request>& req);
};
}