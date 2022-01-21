

#include "sflib/http_part_router.h"
#include "sflib/http_router.h"
#include "sflib/http_server_request.h"
#include "sflib/http_server_response.h"
#include "sflib/router_shared_ptr_compare.h"
#include "sflib/stdc++.h"
#include "sflib/utils.h"
#include <utility>
namespace skyfire
{
http_part_router::http_part_router(
    std::string                                                            prefix,
    std::function<bool(const http_server_request&, http_server_response&)> callback,
    std::vector<std::string> methods, int priority)
    : priority__(priority)
    , methods__(std::move(methods))
    , callback__(std::move(callback))
{
    if (!string_end_with(prefix, "/"))
    {
        prefix__.push_back(prefix + "/");
    }
    prefix__.push_back(prefix);
}
bool http_part_router::run_route(const http_server_request& req,
                                 http_server_response&      res,
                                 const std::string&         url,
                                 const std::string&         method)
{

    std::unique_lock lck(methods_mu__);
    using namespace std::literals;
    if (methods__.cend() == std::find(methods__.cbegin(), methods__.cend(), "*"s))
    {
        if (methods__.cend() == std::find(methods__.cbegin(), methods__.cend(), method))
        {

            return false;
        }
    }
    bool        match_flag = false;
    std::string matched_prefix;
    for (auto& p : prefix__)
    {
        if (string_start_with(url, p))
        {
            matched_prefix = p;
            match_flag     = true;
            break;
        }
    }
    if (!match_flag)
    {
        return false;
    }
    else
    {
        auto new_str = std::string(url.begin() + matched_prefix.size(), url.end());
        if (!new_str.empty() && matched_prefix.back() != '/' && new_str[0] != '/')
        {
            return false;
        }
    }
    if (callback__(req, res))
    {
        auto new_url = "/" + std::string(url.begin() + matched_prefix.size(), url.end());
        bool match   = false;
        for (auto& p : endpoint_router__)
        {
            if (p->run_route(req, res, new_url, method))
            {
                match = true;
                break;
            }
        }
        if (!match)
        {
            match = false;
            for (auto& p : middle_router__)
            {
                if (p->run_route(req, res, new_url, method))
                {
                    match = true;
                    break;
                }
            }
            if (!match)
            {
                res.set_status(404);
            }
        }
    }
    return true;
}
int  http_part_router::priority() const { return priority__; }
bool http_part_router::operator<(
    const http_part_router& router) const
{
    return priority__ < router.priority__;
}
void http_part_router::add_router(
    std::shared_ptr<http_part_router> router)
{
    middle_router__.insert(router);
}
void http_part_router::add_router(std::shared_ptr<router> router)
{
    endpoint_router__.insert(router);
}
} // namespace skyfire
