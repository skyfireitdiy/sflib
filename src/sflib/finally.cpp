
#include "sflib/finally.h"
#include "sflib/stdc++.h"
namespace skyfire
{
__finally__::__finally__(std::function<void()> func)
    : func__(std::move(func))
{
}
__finally__::~__finally__() { func__(); }
} // namespace skyfire