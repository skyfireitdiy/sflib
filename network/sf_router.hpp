
/**
* @version 1.0.0
* @author skyfire
* @mail skyfireitdiy@hotmail.com
* @see http://github.com/skyfireitdiy/sflib
* @file sf_router.hpp

* sflib第一版本发布
* 版本号1.0.0
* 发布日期：2018-10-22
*/
#pragma once
#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCUnusedGlobalDeclarationInspection"

#include "sf_router.h"

namespace skyfire {

inline int sf_router::priority() const { return 0; }

inline bool sf_router::run_route(const sf_http_request &req,
                                 sf_http_response &res, const std::string &url,
                                 const std::string &method) {
    return false;
}
}    // namespace skyfire
#pragma clang diagnostic pop