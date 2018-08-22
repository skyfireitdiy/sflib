#pragma once

#include <map>
#include <string>
#include <sf_utils.h>
#include "sf_type.hpp"
#include "sf_http_utils.hpp"

namespace skyfire{
    class sf_http_header{
        std::map<std::string,std::string> header_data__;
    public:
        void clear();

        void set_header(std::string key, const std::string& value);

        std::string get_header_value(std::string key, const std::string& default_value = "") const;

        void remove_header(std::string key);

        std::vector<std::string> get_key_list() const;
    };
}