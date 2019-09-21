#pragma once

#include "core/sf_define.h"
#include "core/sf_stdc++.h"
#include "tools/sf_nocopy.h"
#include "tools/sf_utils.h"

namespace skyfire {
/**
 * @brief 内存缓存
 *
 */
class sf_cache
    : public sf_make_instance_t<sf_cache, sf_nocopy<sf_empty_class>> {
private:
    struct cache_data_t {
        std::string key;
        std::chrono::system_clock::time_point timestamp_access;
        std::any data;
    };
    std::vector<cache_data_t> data__;
    size_t max_count__;
    std::recursive_mutex mu_data__;

public:
    /**
     * @brief Construct a new sf cache object
     *
     * @param max_count 缓存条目数量
     */
    explicit sf_cache(int max_count = default_cache_max_count);
    /**
     * @brief 取得缓存数据
     *
     * @tparam T 数据类型（如果数据类型不符合会抛出bad_cast异常）
     * @param key 键
     * @return std::shared_ptr<T> 数据
     */
    template <typename T>
    std::shared_ptr<T> data(const std::string& key);
    /**
     * @brief 设置缓存
     *
     * @tparam T 类型
     * @param key 键
     * @param d 值
     */
    template <typename T>
    void set_data(const std::string& key, const T& d);
};
} // namespace skyfire
