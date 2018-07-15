#pragma once

#include <map>
#include <string>
#include "sf_type.h"

namespace skyfire{
    class sf_http_header{
        std::map<std::string,std::string> header_data__;
    public:
        void clear()
        {
            header_data__.clear();
        }

        void set_header(const std::string& key, const std::string& value)
        {
            header_data__[key] = value;
        }

        std::string get_header_value(const std::string& key, const std::string& default_value = "") const
        {
            if(header_data__.count(key) == 0)
                return default_value;
            return header_data__.at(key);
        }

        void remove_header(const std::string& key)
        {
            header_data__.erase(key);
        }

        std::vector<std::string> get_key_list() const
        {
            std::vector<std::string> keys;
            for(auto &p:header_data__)
            {
                keys.push_back(p.first);
            }
            return keys;
        }
    };
}