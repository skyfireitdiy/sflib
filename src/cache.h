
#pragma once
#include "define.h"
#include "nocopy.h"
#include "stdc++.h"
#include "utils.h"
namespace skyfire
{
class cache
    : public make_instance_t<cache, nocopy<empty_class>>
{
private:
    struct cache_data_t
    {
        std::chrono::system_clock::time_point timestamp_access;
        std::any                              data;
    };
    std::unordered_map<std::string, cache_data_t> data__;
    size_t                                        max_count__;
    std::recursive_mutex                          mu_data__;

public:
    explicit cache(int max_count = default_cache_max_count);
    template <typename T>
    std::shared_ptr<T> data(const std::string& key);
    template <typename T>
    void set_data(const std::string& key, const T& d);
};
} // namespace skyfire
