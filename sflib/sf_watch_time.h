
/**
* @version 1.0.0
* @author skyfire
* @file sf_watch_time.h
*/

/*
 * watch_time 时间监测类型
 */
#pragma once
#pragma clang diagnostic push
#pragma ide diagnostic   ignored "OCUnusedGlobalDeclarationInspection"
#pragma ide diagnostic   ignored "OCUnusedStructInspection"

#include "sf_stdc++.h"

namespace skyfire
{
template <typename>
class check_point;

template <typename T = std::string>
class watch_time
{
public:
    void watch();

protected:
    std::string to_string();

    std::unordered_map<T, std::unordered_map<std::thread::id, long long>>
        data__;
    friend class check_point<T>;
};

/**
 * @brief 检查点
 * 
 * @tparam std::string 检查点标识类型
 */

template <typename T = std::string>
class check_point
{
private:
    T                                              point_name__;
    std::chrono::high_resolution_clock::time_point clock__;
    watch_time<T>&                                 parent__;

public:
    /**
     * @brief check_point 设置检查点
     * @param name 检查点名称
     * @param parent 对象
     */
    check_point(const T& name, watch_time<T>& parent);

    ~check_point();
};
} // namespace skyfire

#pragma clang diagnostic pop