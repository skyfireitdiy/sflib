
#pragma once
#include "sf_stdc++.h"
#include "sf_utils.hpp"
#include "sf_watch_time.h"
namespace skyfire
{
template <typename T>
check_point<T>::~check_point()
{
    parent__.data__[point_name__][std::this_thread::get_id()] += std::chrono::duration_cast<std::chrono::nanoseconds>(
                                                                     std::chrono::high_resolution_clock::now() - clock__)
                                                                     .count();
}

template <typename T>
check_point<T>::check_point(const T& name, watch_time<T>& parent)
    : point_name__(name)
    , clock__(std::chrono::high_resolution_clock::now())
    , parent__(parent)
{
}
template <typename T>
void watch_time<T>::watch()
{
    std::cout << to_string() << std::flush;
}
template <typename T>
std::string watch_time<T>::to_string()
{
    std::ostringstream so;
    auto               old = so.flags();
    so << std::right;
    so << std::setw(55) << "SkyFire Time Watch" << std::endl;
    ;
    so << "===================================================================="
          "====================="
       << std::endl;
    so << std::left;
    so << std::setw(30) << "Points" << std::setw(6) << "|" << std::setw(12)
       << "Thread" << std::setw(20) << "|"
       << "Time" << std::endl;
    so << "===================================================================="
          "====================="
       << std::endl;
    for (auto& p : data__)
    {
        for (auto& q : p.second)
        {
            so << std::setw(30) << p.first << std::setw(6) << "|"
               << std::setw(12) << q.first << std::setw(6) << "|"
               << convert_ns_to_readable(q.second) << std::endl;
        }
    }
    so << "--------------------------------------------------------------------"
          "---------------------"
       << std::endl;
    so.flags(old);
    return so.str();
}
} // namespace skyfire
