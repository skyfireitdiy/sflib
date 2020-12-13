
/**
* @version 1.0.0
* @author skyfire
* @file sf_cache.h

* 
*/

#pragma once

#include "sf_define.h"
#include "sf_nocopy.h"
#include "sf_stdc++.h"
#include "sf_utils.h"

namespace skyfire
{
/**
 * @brief 内存缓存
 *
 */
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
    /**
     * @brief Construct a new sf cache object
     *
     * @param max_count 缓存条目数量
     */
    explicit cache(int max_count = default_cache_max_count);
    /**
     * @brief 取得缓存数据
     *
     * @tparam T 数据类型（如果数据类型不匹配会返回nullptr）
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
