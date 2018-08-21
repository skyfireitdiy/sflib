#pragma once

#include <memory>

#include "sf_http_router.h"
#include "sf_websocket_router.h"


namespace skyfire
{
    template <typename RouterType>
    struct sf_router_shared_ptr_compare__
    {
        bool operator()(const std::shared_ptr<RouterType> &router1, const std::shared_ptr<RouterType> &router2) const
        {
            return router1->get_priority() < router2->get_priority();
        }
    };
} // namespace skyfire