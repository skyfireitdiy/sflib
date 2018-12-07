#pragma once

#include <string>
#include <vector>
#include <any>
#include <unordered_map>

namespace skyfire
{
    enum class object_item_scope_t
    {
        singleton,
        prototype
    };
    
    using object_properties_t = std::unordered_map<std::string, std::any>;
    
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