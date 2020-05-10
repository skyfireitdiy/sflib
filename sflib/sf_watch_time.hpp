
/**
* @version 1.0.0
* @author skyfire
* @file sf_watch_time.hpp
*/

/*
 * sf_watch_time 时间监测类型
 */
#pragma once
#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCUnusedGlobalDeclarationInspection"

#include "sf_watch_time.h"
#include "sf_utils.hpp"

namespace skyfire {
template <typename T>
sf_check_point<T>::~sf_check_point()
{
    parent__.data__[point_name__][std::this_thread::id()] += std::chrono::duration_cast<std::chrono::nanoseconds>(
        std::chrono::high_resolution_clock::now() - clock__)
                                                                 .count();
}

template <typename T>
sf_check_point<T>::sf_check_point(const T& name, sf_watch_time<T>& parent)
    : point_name__(name)
    , clock__(std::chrono::high_resolution_clock::now())
    , parent__(parent)
{
}

template <typename T>
void sf_watch_time<T>::watch()
{
    std::cout << to_string() << std::flush;
}

template <typename T>
std::string sf_watch_time<T>::to_string()
{
    std::ostringstream so;
    auto old = so.flags();

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

    for (auto& p : data__) {
        for (auto& q : p.second) {
            so << std::setw(30) << p.first << std::setw(6) << "|"
               << std::setw(12) << q.first << std::setw(6) << "|"
               << sf_convert_ns_to_readable(q.second) << std::endl;
        }
    }
    so << "--------------------------------------------------------------------"
          "---------------------"
       << std::endl;
    so.flags(old);
    return so.str();
}
} // namespace skyfire

#pragma clang diagnostic pop