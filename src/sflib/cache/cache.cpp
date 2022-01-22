
#include "sflib/cache/cache.h"
#include "sflib/tools/finally.h"
#include "sflib/tools/nocopy.h"
#include "sflib/tools/utils.h"

namespace skyfire
{
cache::cache(int max_count)
    : max_count__(max_count)
{
    if (max_count__ == 0)
    {
        max_count__ = 1;
    }
}
} // namespace skyfire
