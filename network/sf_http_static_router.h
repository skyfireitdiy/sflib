
/**
* @version 1.0.0
* @author skyfire
* @mail skyfireitdiy@hotmail.com
* @see http://github.com/skyfireitdiy/sflib
* @file sf_http_static_router.h

* sflib第一版本发布
* 版本号1.0.0
* 发布日期：2018-10-22
*/

#pragma once

#include <memory>
#include <string>
#include "core/sf_define.h"
#include "sf_http_router.hpp"
#include "sf_http_utils.hpp"
#include "tools/sf_logger.hpp"
#include "tools/sf_utils.h"

namespace skyfire {
using namespace std::literals;

class sf_static_router
    : public sf_make_instance_t<sf_static_router, sf_router> {
   private:
    int priority__ = default_http_static_priority;
    std::string static_path__;
    std::vector<std::string> methods__;
    std::function<void(const sf_http_request &, sf_http_response &,
                       const std::string &, const std::string &)>
        callback__;

   public:
    bool run_route(const sf_http_request &req, sf_http_response &res,
                   const std::string &url, const std::string &method) override;

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
    explicit sf_static_router(std::string path,
                              std::vector<std::string> methods = {{"GET"s}},
                              std::string charset = "utf-8",
                              bool deflate = true,
                              int priority = default_http_static_priority);
};

}    // namespace skyfire