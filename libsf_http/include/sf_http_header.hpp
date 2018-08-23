#pragma once

#include "sf_http_header.h"

namespace skyfire{

    inline void sf_http_header::clear() {
        header_data__.clear();
    }

    inline std::vector<std::string> sf_http_header::get_key_list() const {
        std::vector<std::string> keys;
        for(auto &p:header_data__)
        {
            keys.push_back(p.first);
        }
        return keys;
    }

    inline void sf_http_header::remove_header(std::string key) {
        key = sf_to_header_key_format(key);
        header_data__.erase(key);
    }

    inline std::string sf_http_header::get_header_value(std::string key, const std::string &default_value) const {
        key = sf_to_header_key_format(key);
        if(header_data__.count(key) == 0)
            return default_value;
        return header_data__.at(key);
    }

    inline void sf_http_header::set_header(std::string key, const std::string &value) {
        key = sf_to_header_key_format(key);
        header_data__[key] = value;
    }
}