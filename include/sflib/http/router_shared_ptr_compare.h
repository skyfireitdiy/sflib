
#pragma once

namespace skyfire
{
template <typename RouterType>
struct router_shared_ptr_compare__ final
{
    bool operator()(const std::shared_ptr<RouterType> &router1,
                    const std::shared_ptr<RouterType> &router2) const;
};

template <typename RouterType>
bool router_shared_ptr_compare__<RouterType>::operator()(
    const std::shared_ptr<RouterType> &router1,
    const std::shared_ptr<RouterType> &router2) const
{
    return router1->priority() < router2->priority();
}
} // namespace skyfire