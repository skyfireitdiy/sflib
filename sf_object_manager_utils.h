#pragma once

#include <string>
#include <vector>
#include <any>
#include <unordered_map>
#include "sf_json.hpp"

namespace skyfire
{
    enum class object_item_scope_t
    {
        singleton,
        prototype
    };

    enum class object_value_type_t{
        value,
        ref
    };
    
    struct object_property_t
    {
        object_value_type_t type;
        sf_json data;
    };  
    
    using object_properties_t = std::unordered_map<std::string, object_property_t>;
    
    struct object_item_meta_t
    {
        std::string id;
        object_item_scope_t scope;
        std::string _class;
        object_properties_t properties;
    };
    
    struct object_manager_meta_t
    {
        std::unordered_map<std::string,object_item_meta_t> objects;
    };
}