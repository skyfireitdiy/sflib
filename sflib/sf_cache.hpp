
#pragma once
#include "sf_cache.h"
#include "sf_finally.hpp"
#include "sf_logger.hpp"
#include "sf_nocopy.h"
#include "sf_utils.hpp"
namespace skyfire
{
inline cache::cache(int max_count)
    : max_count__(max_count)
{
    if (max_count__ == 0)
    {
        max_count__ = 1;
    }
}
template <typename T>
std::shared_ptr<T> cache::data(const std::string& key)
{
    std::lock_guard<std::recursive_mutex> lck(mu_data__);
    if (data__.contains(key))
    {
        data__[key].timestamp_access = std::chrono::system_clock::now();
        try
        {
            return std::make_shared<T>(std::any_cast<T>(data__[key].data));
        }
        catch (const std::bad_cast&)
        {
            return nullptr;
        }
    }
    return nullptr;
}
template <typename T>
void cache::set_data(const std::string& key, const T& d)
{
    cache_data_t                          tmp_cache { std::chrono::system_clock::now(), std::decay_t<T>(d) };
    std::lock_guard<std::recursive_mutex> lck(mu_data__);
    if (data__.contains(key))
    {
        data__[key] = tmp_cache;
        return;
    }
    if (data__.size() < max_count__)
    {
        data__[key] = tmp_cache;
        return;
    }
    auto        min_tm    = data__.begin()->second.timestamp_access;
    std::string min_index = data__.begin()->first;
    for (auto& p : data__)
    {
        if (min_tm > p.second.timestamp_access)
        {
            min_tm    = p.second.timestamp_access;
            min_index = p.first;
        }
    }
    data__.erase(min_index);
    data__[key] = tmp_cache;
}
} // namespace skyfire
