#pragma once

#include <map>
#include <string>
#include <sf_utils.hpp>
#include "sf_type.hpp"
#include "sf_http_utils.hpp"

namespace skyfire{
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
    };
}