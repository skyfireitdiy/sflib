

#include "sflib/http/http_header.h"
#include "sflib/http/http_utils.h"
namespace skyfire
{
void http_header::clear()
{
    header_data__.clear();
}
std::vector<std::string> http_header::key_list() const
{
    std::vector<std::string> keys;
    for (auto &p : header_data__)
    {
        keys.push_back(p.first);
    }
    return keys;
}
void http_header::remove_header(std::string key)
{
    key = to_header_key_format(key);
    header_data__.erase(key);
}
std::string http_header::header_value(
    std::string key, const std::string &default_value) const
{
    key = to_header_key_format(key);
    if (header_data__.count(key) == 0)
        return default_value;
    return header_data__.at(key);
}
void http_header::set_header(std::string key,
                             const std::string &value)
{
    key = to_header_key_format(key);
    header_data__[key] = value;
}
http_header_t http_header::header() const
{
    return header_data__;
}
void http_header::set_header(const http_header_t &header)
{
    header_data__ = header;
}
bool http_header::has_key(const std::string &key) const
{
    return header_data__.count(to_header_key_format(key)) != 0;
}
} // namespace skyfire
