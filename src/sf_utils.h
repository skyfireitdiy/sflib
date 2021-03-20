
#pragma once
#include "sf_empty_class.h"
#include "sf_stdc++.h"
#include "sf_type.hpp"
#include <sys/stat.h>
#include <sys/types.h>
namespace skyfire
{
inline std::string              string_trim(const std::string& str);
inline std::string              to_lower_string(std::string str);
inline std::string              get_path_ext(const std::string& path);
inline bool                     equal_nocase_string(const std::string& str1,
                                                    const std::string& str2);
inline std::vector<std::string> split_string(std::string        str,
                                             const std::string& split_str);
bool                            write_file(const std::string& file_name, const byte_array& data,
                                           bool append);
bool                            read_file(const std::string& file_name, byte_array& data);
void                            string_replace(std::string& str, const std::string& from,
                                               const std::string& to);
bool                            string_start_with(const std::string& str, const std::string& prefix);
bool                            string_end_with(const std::string& str, const std::string& suffix);
std::string                     make_time_str(const std::chrono::system_clock::time_point& tp  = std::chrono::system_clock::now(),
                                              const std::string&                           fmt = "%Y-%m-%d %H:%M:%S");
std::string                     make_time_str(const std::filesystem::file_time_type& tp, const std::string& fmt = "%Y-%m-%d %H:%M:%S");
std::string                     long_double_to_string(const long double& num);
template <typename T>
std::string char_container_to_hex_string(const T& data);
template <typename T>
void        hex_string_to_char_container(const std::string& str, T& data);
std::string convert_ns_to_readable(long long time);
template <typename T, typename Base = empty_class>
struct make_instance_t : public Base
{
private:
    make_instance_t() = default;

public:
    template <typename... Args>
    static std::shared_ptr<T> make_instance(Args&&... args)
    {
        return std::shared_ptr<T>(new T(std::forward<Args>(args)...));
    }
    using Base::Base;
    friend T;
};
std::string safe_path(std::string danger_path);
} // namespace skyfire