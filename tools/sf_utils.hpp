
/**
* @version 1.0.0
* @author skyfire
* @mail skyfireitdiy@hotmail.com
* @see http://github.com/skyfireitdiy/sflib
* @file sf_utils.hpp

* sflib第一版本发布
* 版本号1.0.0
* 发布日期：2018-10-22
*/
#pragma once
#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCUnusedGlobalDeclarationInspection"

#include <sys/stat.h>
#include <sys/types.h>
#include "core/sf_stdc++.h"

#include "core/sf_msvc_safe.h"
#include "core/sf_type.hpp"
#include "sf_utils.h"

namespace skyfire {
inline std::string sf_string_trim(const std::string &str) {
    if (str.length() == 0) return "";
    unsigned long begin = 0;
    for (unsigned long i = 0; i < str.length(); ++i) {
        if (!isspace(str[i])) {
            begin = i;
            break;
        }
    }
    unsigned long end = begin;
    for (unsigned long i = str.length(); i > begin; --i) {
        if (!isspace(str[i - 1])) {
            end = i;
            break;
        }
    }
    return {str.begin() + begin, str.begin() + end};
}

inline std::string sf_to_lower_string(std::string str) {
    for (auto &p : str) p = static_cast<char>(tolower(p));
    return str;
}

inline std::string sf_get_path_ext(const std::string &path) {
    auto pos = path.rfind('.');
    if (pos == -1) return "";
    return std::string(path.begin() + pos + 1, path.end());
}

inline bool sf_equal_nocase_string(const std::string &str1,
                                   const std::string &str2) {
    return sf_to_lower_string(str1) == sf_to_lower_string(str2);
}

inline std::vector<std::string> sf_split_string(std::string str,
                                                const std::string &split_str) {
    std::vector<std::string> ret;
    unsigned long pos = 0;
    while (std::string::npos != (pos = str.find(split_str))) {
        ret.emplace_back(str.begin(), str.begin() + pos);
        str = std::string(str.begin() + pos + split_str.length(), str.end());
    }
    ret.push_back(str);
    return ret;
}

inline bool sf_write_file(const std::string &file_name, const byte_array &data,
                          bool append) {
    std::ofstream fo(
        file_name, (append ? std::ios::app : std::ios::out) | std::ios::binary);
    if (!fo) return false;
    fo.write(data.data(), data.size());
    fo.close();
    return true;
}

inline bool sf_read_file(const std::string &file_name, byte_array &data) {
    std::ifstream fi(file_name, std::ios::in | std::ios::binary);
    if (!fi) return false;
    fi.seekg(0, std::ios::end);
    auto size = fi.tellg();
    data.resize(static_cast<unsigned long>(size));
    fi.seekg(0, std::ios::beg);
    fi.read(data.data(), size);
    fi.close();
    return true;
}

inline void sf_string_replace(std::string &str, const std::string &from,
                              const std::string &to) {
    if (str.empty()) return;
    size_t start_pos = 0;
    while ((start_pos = str.find(from, start_pos)) != std::string::npos) {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length();
    }
}

inline std::string sf_long_double_to_string(const long double &num) {
    auto buffer = std::to_string(num);
    if (buffer.length() > 7) {
        if (std::string(buffer.end() - 7, buffer.end()) == ".000000") {
            buffer = {buffer.begin(), buffer.end() - 7};
        }
    }
    return buffer;
}

inline long double sf_string_to_long_double(const std::string &str) {
    long double tmp_num;
#ifdef MSC_VER
    sscanf_s(str.c_str(), "%Lf", &tmp_num);
#else
    sscanf(str.c_str(), "%Lf", &tmp_num);
#endif    // MSC_VER
    return tmp_num;
}

#pragma clang diagnostic push
#pragma ide diagnostic ignored "UnusedValue"

inline std::string sf_make_time_str(
    const std::chrono::system_clock::time_point &tp) {
    auto tt = std::chrono::system_clock::to_time_t(tp);
    tm tm_d;
    auto ptm = &tm_d;
#ifdef _MSC_VER
    localtime_s(ptm, &tt);
#else
    ptm = localtime(&tt);
#endif
    std::ostringstream os;
    os << std::put_time(ptm, "%Y-%m-%d %H:%M:%S");
    return os.str();
}

#pragma clang diagnostic pop

template <typename T>
std::string sf_char_container_to_hex_string(const T &data) {
    std::string str;
    str.reserve(data.size() * 2 + 1);
    char tmp[4];
    for (auto &p : data) {
        sf_safe_sprintf(tmp, 4, "%02x", p);
        str += tmp;
    }
    return str;
}

template <typename T>
void sf_hex_string_to_char_container(const std::string &str, T &data) {
    data.clear();
    if (str.size() % 2 != 0) {
        return;
    }
    std::vector<char> tmp_ret_data(str.size() / 2);
    for (auto i = 0; i < str.length() / 2; ++i) {
        int tmp;
#ifdef _MSC_VER
        sscanf_s(str.data() + i * 2, "%02x", &tmp);
#else
        sscanf(str.data() + i * 2, "%02x", &tmp);
#endif    // _MSC_VER
        tmp_ret_data[i] = tmp;
    }
    data = T(tmp_ret_data.begin(), tmp_ret_data.end());
}

inline bool sf_string_start_with(const std::string &str,
                                 const std::string &prefix) {
    return str.find(prefix) == 0;
}

inline bool sf_string_end_with(const std::string &str,
                               const std::string &suffix) {
    return str.find_last_of(suffix) == str.length() - suffix.size();
}

inline std::string sf_safe_path(std::string danger_path) {
    sf_string_replace(danger_path,
                      std::string(1, fs::path::preferred_separator), "");
    return danger_path;
}

}    // namespace skyfire
#pragma clang diagnostic pop