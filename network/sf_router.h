
/**
* @version 1.0.0
* @author skyfire
* @file sf_router.h
*/
#pragma once
#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCUnusedGlobalDeclarationInspection"

namespace skyfire {
/**
 *  @brief 路由
 */
class sf_router {
   public:
    virtual ~sf_router() = default;
    /**
     * 获取优先级
     * @return 优先级
     */
    virtual int priority() const;

    virtual bool run_route(const sf_http_request &req, sf_http_response &res,
                           const std::string &url, const std::string &method);
};

}    // namespace skyfire
#pragma clang diagnostic pop