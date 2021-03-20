
#pragma once
#include "sf_single_instance.h"
#include "sf_stdc++.h"
namespace skyfire
{
template <typename ThisClass, typename BaseClass>
template <typename... Args>
std::shared_ptr<ThisClass> single_instance<ThisClass, BaseClass>::instance(
    Args&&... args)
{
    static auto instance__ = std::shared_ptr<ThisClass>(
        new ThisClass(std::forward<Args>(args)...));
    return instance__;
}
} // namespace skyfire
