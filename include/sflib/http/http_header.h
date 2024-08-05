
#pragma once

#include <string>
#include <unordered_map>
#include <vector>


namespace skyfire
{
using http_header_t = std::unordered_map<std::string, std::string>;
class http_header
{
protected:
    http_header_t header_data__;

public:
    void                     clear();
    void                     set_header(std::string key, const std::string& value);
    std::string              header_value(std::string        key,
                                          const std::string& default_value = "") const;
    void                     remove_header(std::string key);
    std::vector<std::string> key_list() const;
    http_header_t            header() const;
    void                     set_header(const http_header_t& header);
    bool                     has_key(const std::string& key) const;
};
} // namespace skyfire
