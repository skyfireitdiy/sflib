
#include "cache.h"
#include "finally.h"
#include "logger.h"
#include "nocopy.h"
#include "utils.h"
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
