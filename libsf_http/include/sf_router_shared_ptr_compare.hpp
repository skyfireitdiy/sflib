#pragma once

#include "sf_router_shared_ptr_compare.h"

namespace skyfire
{

    template<typename RouterType>
    bool sf_router_shared_ptr_compare__<RouterType>::operator()(const std::shared_ptr<RouterType> &router1,
                                                                const std::shared_ptr<RouterType> &router2) const {
        return router1->get_priority() < router2->get_priority();
    }
} // namespace skyfire