
#pragma once
#include "router_shared_ptr_compare.h"
namespace skyfire
{
template <typename RouterType>
bool router_shared_ptr_compare__<RouterType>::operator()(
    const std::shared_ptr<RouterType>& router1,
    const std::shared_ptr<RouterType>& router2) const
{
    return router1->priority() < router2->priority();
}
} // namespace skyfire