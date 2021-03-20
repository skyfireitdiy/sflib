
#pragma once
#include "define.h"
#include "http_router.hpp"
#include "http_utils.hpp"
#include "logger.hpp"
#include "utils.h"
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
    bool              run_route(const http_server_request& req, http_server_response& res,
                                const std::string& url, const std::string& method) override;
    [[nodiscard]] int priority() const override;
    explicit static_router(std::string              path,
                           std::vector<std::string> methods  = { { "GET"s } },
                           std::string              charset  = "utf-8",
                           bool                     deflate  = true,
                           int                      priority = default_http_static_priority);
};
} // namespace skyfire