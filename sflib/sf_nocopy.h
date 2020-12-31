
#pragma once
#include "sf_empty_class.h"
namespace skyfire
{
template <typename _BaseClass = empty_class>
class nocopy : public _BaseClass
{
public:
    nocopy() {};
    nocopy(const nocopy&) = delete;
    nocopy(nocopy&&)      = delete;
    nocopy& operator=(const nocopy&) = delete;
    nocopy& operator=(nocopy&&) = delete;
};
} // namespace skyfire
