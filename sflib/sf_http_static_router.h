
/**
* @version 1.0.0
* @author skyfire
* @file sf_http_static_router.h
*/

#pragma once

#include "sf_define.h"
#include "sf_http_router.hpp"
#include "sf_http_utils.hpp"
#include "sf_logger.hpp"
#include "sf_utils.h"
#include <memory>
#include <string>

namespace skyfire
{
using namespace std::literals;

class static_router
    : public make_instance_t<static_router, router>
{
private:
    std::string              static_path__;
    std::vector<std::string> methods__;
    int                      priority__ = default_http_static_priority;
    std::function<void(const http_server_request&, http_server_response&,
                       const std::string&, const std::string&)>
        callback__;

public:
    bool run_route(const http_server_request& req, http_server_response& res,
                   const std::string& url, const std::string& method) override;

    [[nodiscard]] int priority() const override;

    /**
     * @brief Construct a new sf static router object
     *
     * @param path 路径
     * @param methods 请求方法
     * @param charset 编码
     * @param deflate 是否启用压缩
     * @param priority 优先级
     */
    explicit static_router(std::string              path,
                           std::vector<std::string> methods  = { { "GET"s } },
                           std::string              charset  = "utf-8",
                           bool                     deflate  = true,
                           int                      priority = default_http_static_priority);
};

} // namespace skyfire