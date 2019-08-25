
/**
* @version 1.0.0
* @author skyfire
* @mail skyfireitdiy@hotmail.com
* @see http://github.com/skyfireitdiy/sflib
* @file sf_type.hpp

* sflib第一版本发布
* 版本号1.0.0
* 发布日期：2018-10-22
*/

/*
 * 常用类型声明
 */

#pragma once

#include "sf_type.h"

namespace skyfire {

inline std::string to_string(const byte_array &data) {
    return std::string(data.begin(), data.end());
}

inline byte_array operator+(byte_array b1, const byte_array &b2) {
    b1.insert(b1.end(), b2.begin(), b2.end());
    return b1;
}

inline byte_array &operator+=(byte_array &b1, const byte_array &b2) {
    b1.insert(b1.end(), b2.begin(), b2.end());
    return b1;
}

template <typename T>
inline byte_array to_byte_array(const T &t) {
    byte_array ret(sizeof(t));
    std::memcpy(ret.data(), &t, sizeof(t));
    return ret;
}

inline byte_array to_byte_array(const std::string &str) {
    byte_array ret(str.length());
    std::memcpy(ret.data(), str.data(), str.length());
    return ret;
}

#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCUnusedGlobalDeclarationInspection"
inline bool dump_byte_array(const byte_array &data, const std::string &filename,
                            bool app) {
    std::ofstream fo(filename, app ? (std::ios::app | std::ios::binary)
                                   : (std::ios::trunc | std::ios::out));
    if (!fo) return false;
    fo.write(data.data(), data.size());
    return true;
}
#pragma clang diagnostic pop

}    // namespace skyfire
