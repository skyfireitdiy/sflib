
/**
* @version 1.0.0
* @author skyfire
* @mail skyfireitdiy@hotmail.com
* @see http://github.com/skyfireitdiy/sflib
* @file sf_watch_time.hpp

* sflib第一版本发布
* 版本号1.0.0
* 发布日期：2018-10-22
*/

/*
 * sf_watch_time 时间监测类型
 */
#pragma once
#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCUnusedGlobalDeclarationInspection"

#include "sf_watch_time.h"

namespace skyfire {
template <typename T>
sf_check_point<T>::~sf_check_point() {
    parent__.data__[point_name__][std::this_thread::id()] +=
        std::chrono::duration_cast<std::chrono::nanoseconds>(
            std::chrono::high_resolution_clock::now() - clock__)
            .count();
}

template <typename T>
sf_check_point<T>::sf_check_point(const T &name, sf_watch_time<T> &parent)
    : point_name__(name),
      clock__(std::chrono::high_resolution_clock::now()),
      parent__(parent) {}

template <typename T>
void sf_watch_time<T>::watch() {
    std::cout << to_string() << std::flush;
}

template <typename T>
std::string sf_watch_time<T>::convert_ms_to_readable__(long long time) const {
    std::ostringstream so;
    auto old = so.flags();
    so << std::left;
    const long long ns = 1;
    const long long us = 1000;
    const auto ms = 1000 * us;
    const auto s = 1000 * ms;
    const auto m = 60 * s;
    const auto h = 60 * m;
    const auto d = 24 * h;

    if (time >= d) {
        so << std::to_string(time / d) + "d";
        time %= d;
    } else {
        so << "";
    }
    so << std::setw(6);
    if (time >= h) {
        so << std::to_string(time / h) + "h";
        time %= h;
    } else {
        so << "";
    }
    so << std::setw(6);
    if (time >= m) {
        so << std::to_string(time / m) + "m";
        time %= m;
    } else {
        so << "";
    }
    so << std::setw(6);
    if (time >= s) {
        so << std::to_string(time / s) + "s";
        time %= s;
    } else {
        so << "";
    }
    so << std::setw(6);
    if (time >= ms) {
        so << std::to_string(time / ms) + "ms";
        time %= ms;
    } else {
        so << "";
    }
    so << std::setw(6);
    if (time >= us) {
        so << std::to_string(time / us) + "us";
        time %= us;
    } else {
        so << "";
    }
    so << std::setw(6);
    if (time >= ns) {
        so << std::to_string(time / ns) + "ns";
    }
    so.setf(old);
    return so.str();
}

template <typename T>
std::string sf_watch_time<T>::to_string() {
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

    for (auto &p : data__) {
        for (auto &q : p.second) {
            so << std::setw(30) << p.first << std::setw(6) << "|"
               << std::setw(12) << q.first << std::setw(6) << "|"
               << convert_ms_to_readable__(q.second) << std::endl;
        }
    }
    so << "--------------------------------------------------------------------"
          "---------------------"
       << std::endl;
    so.flags(old);
    return so.str();
}
}    // namespace skyfire

#pragma clang diagnostic pop