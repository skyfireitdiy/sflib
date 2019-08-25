
/**
* @version 1.0.0
* @author skyfire
* @mail skyfireitdiy@hotmail.com
* @see http://github.com/skyfireitdiy/sflib
* @file sf_watch_time.h

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
#pragma ide diagnostic ignored "OCUnusedStructInspection"

#include "core/sf_stdc++.h"

namespace skyfire {
template <typename>
class sf_check_point;

template <typename T = std::string>
class sf_watch_time {
   public:
    void watch();

   protected:
    std::string to_string();

    std::unordered_map<T, std::unordered_map<std::thread::id, long long>>
        data__;
    std::string convert_ms_to_readable__(long long time) const;

    friend class sf_check_point<T>;
};

template <typename T = std::string>
class sf_check_point {
   private:
    T point_name__;
    std::chrono::high_resolution_clock::time_point clock__;
    sf_watch_time<T> &parent__;

   public:
    /**
     * @brief sf_check_point 设置检查点
     * @param name 检查点名称
     * @param parent 对象
     */
    sf_check_point(const T &name, sf_watch_time<T> &parent);

    ~sf_check_point();
};
}    // namespace skyfire

#pragma clang diagnostic pop