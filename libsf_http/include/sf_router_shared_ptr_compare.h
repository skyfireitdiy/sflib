#pragma once

#include <memory>

#include "sf_http_router.hpp"
#include "sf_websocket_router.hpp"


namespace skyfire
{
    template <typename RouterType>
    struct sf_router_shared_ptr_compare__
    {
        bool operator()(const std::shared_ptr<RouterType> &router1, const std::shared_ptr<RouterType> &router2) const;
    };
} // namespace skyfire