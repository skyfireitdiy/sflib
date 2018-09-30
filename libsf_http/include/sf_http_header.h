#pragma once

#include <string>
#include <map>
#include <vector>
#include "sf_http_utils.h"

namespace skyfire
{
    class sf_http_header{
        sf_http_header_t header_data__;
    public:
        void clear();

        void set_header(std::string key, const std::string& value);

        std::string get_header_value(std::string key, const std::string& default_value = "") const;

        void remove_header(std::string key);

        std::vector<std::string> get_key_list() const;

        sf_http_header_t get_header() const;

        void set_header(const sf_http_header_t& header);

        bool has_key(const std::string& key) const ;

        std::string to_string() const;
    };
}