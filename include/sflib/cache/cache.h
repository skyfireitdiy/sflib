
#pragma once
#include "sflib/tools/define.h"
#include "sflib/tools/nocopy.h"
#include "sflib/tools/utils.h"
#include <any>
#include <mutex>
#include <unordered_map>

namespace skyfire
{
class cache
    : public make_instance_t<cache, nocopy<empty_class>>
{
private:
    struct cache_data_t
    {
        std::chrono::system_clock::time_point timestamp_access;
        std::any data;
    };
    std::unordered_map<std::string, cache_data_t> data__;
    size_t max_count__;
    std::recursive_mutex mu_data__;

public:
    explicit cache(int max_count = default_cache_max_count);
    template <typename T>
    std::shared_ptr<T> data(const std::string &key);
    template <typename T>
    void set_data(const std::string &key, const T &d);
};

template <typename T>
std::shared_ptr<T> cache::data(const std::string &key)
{
    std::lock_guard lck(mu_data__);
    if (data__.contains(key))
    {
        data__[key].timestamp_access = std::chrono::system_clock::now();
        try
        {
            return std::make_shared<T>(std::any_cast<T>(data__[key].data));
        }
        catch (const std::bad_cast &)
        {
            return nullptr;
        }
    }
    return nullptr;
}
template <typename T>
void cache::set_data(const std::string &key, const T &d)
{
    cache_data_t tmp_cache {std::chrono::system_clock::now(), std::decay_t<T>(d)};
    std::lock_guard lck(mu_data__);
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
    auto min_tm = data__.begin()->second.timestamp_access;
    std::string min_index = data__.begin()->first;
    for (auto &p : data__)
    {
        if (min_tm > p.second.timestamp_access)
        {
            min_tm = p.second.timestamp_access;
            min_index = p.first;
        }
    }
    data__.erase(min_index);
    data__[key] = tmp_cache;
}

} // namespace skyfire
