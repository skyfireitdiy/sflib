
#pragma once
#include "stdc++.h"
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
template <typename T = std::string>
class check_point
{
private:
    T                                              point_name__;
    std::chrono::high_resolution_clock::time_point clock__;
    watch_time<T>&                                 parent__;

public:
    check_point(const T& name, watch_time<T>& parent);
    ~check_point();
};
} // namespace skyfire
