
#pragma once
#include "single_instance.h"
#include "stdc++.h"
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
