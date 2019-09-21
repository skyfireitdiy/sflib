#pragma once

#include "sf_cache.h"
#include "tools/sf_nocopy.h"
#include "tools/sf_utils.hpp"

namespace skyfire {
inline sf_cache::sf_cache(int max_count)
    : max_count__(max_count)
{
    data__.reserve(max_count__);
}

template <typename T>
std::shared_ptr<T> sf_cache::data(const std::string& key)
{
    std::lock_guard<std::recursive_mutex> lck(mu_data__);
    for (auto& p : data__) {
        if (p.key == key) {
            p.timestamp_access = std::chrono::system_clock::now();
            return std::make_shared<T>(std::any_cast<T>(p.data));
        }
    }
    return nullptr;
}

template <typename T>
void sf_cache::set_data(const std::string& key, const T& d)
{
    cache_data_t tmp { key, std::chrono::system_clock::now(), d };
    std::lock_guard<std::recursive_mutex> lck(mu_data__);
    if (data__.size() < max_count__) {
        data__.push_back(tmp);
        return;
    }
    auto min_tm = data__[0].timestamp_access;
    auto min_index = 0;
    for (auto i = 1UL; i < data__.size(); ++i) {
        if (data__[i].key == key) {
            data__[i] = tmp;
            return;
        }
        if (min_tm > data__[i].timestamp_access) {
            min_tm = data__[i].timestamp_access;
            min_index = i;
        }
    }
    data__[min_index] = tmp;
}
} // namespace skyfire
