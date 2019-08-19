#pragma once

#include "core/sf_define.h"
#include "tools/sf_nocopy.h"
#include "tools/sf_utils.h"
#include "core/sf_stdc++.h"

namespace skyfire
{
    class sf_cache: public sf_make_instance_t<sf_cache, sf_nocopy<sf_empty_class>>
    {
    private:
        struct cache_data_t{
            std::string key;
            std::chrono::system_clock::time_point timestamp;
            std::any data;
        };
        std::vector<cache_data_t> data__;
        size_t max_count__;
        std::recursive_mutex mu_data__;
    public:
        explicit sf_cache(int max_count = default_cache_max_count);
        template<typename T>
        std::shared_ptr<T> data(const std::string& key);
        template<typename T>
        void set_data(const std::string &key, const T& d);
    };
}