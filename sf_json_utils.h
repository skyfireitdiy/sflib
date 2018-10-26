#pragma once

#include <string>
#include <vector>
#include <memory>
#include <map>

namespace skyfire
{
    enum class sf_json_type
    {
        null,
        boolean,
        number,
        string,
        array,
        object,
    };

    struct sf_json_type_null
    {
    };

    struct sf_json_value
    {
        sf_json_type type;
        long double number_value;
        std::string string_value;
        std::vector<std::shared_ptr<sf_json_value>> array_value;
        std::map<std::string, std::shared_ptr<sf_json_value>> object_value;
    };
}