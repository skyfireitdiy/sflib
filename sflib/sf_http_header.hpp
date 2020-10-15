
/**
* @version 1.0.0
* @author skyfire
* @file sf_http_header.hpp
*/
#pragma once
#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCUnusedGlobalDeclarationInspection"

#include "sf_http_header.h"

namespace skyfire {
inline void sf_http_header::clear() { header_data__.clear(); }

inline std::vector<std::string> sf_http_header::key_list() const
{
    std::vector<std::string> keys;
    for (auto& p : header_data__) {
        keys.push_back(p.first);
    }
    return keys;
}

inline void sf_http_header::remove_header(std::string key)
{
    key = sf_to_header_key_format(key);
    header_data__.erase(key);
}

inline std::string sf_http_header::header_value(
    std::string key, const std::string& default_value) const
{
    key = sf_to_header_key_format(key);
    if (header_data__.count(key) == 0)
        return default_value;
    return header_data__.at(key);
}

inline void sf_http_header::set_header(std::string key,
    const std::string& value)
{
    key = sf_to_header_key_format(key);
    header_data__[key] = value;
}

inline sf_http_header_t sf_http_header::header() const { return header_data__; }

inline void sf_http_header::set_header(const sf_http_header_t& header)
{
    header_data__ = header;
}

inline bool sf_http_header::has_key(const std::string& key) const
{
    return header_data__.count(sf_to_header_key_format(key)) != 0;
}


} // namespace skyfire
#pragma clang diagnostic pop