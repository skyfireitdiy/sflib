#pragma once

#include <string>
#include <functional>
#include <any>
#include "sf_json.h"

namespace skyfire
{
    class sf_object;
    class sf_object_mem_meta_helper
    {
    public:
        sf_object_mem_meta_helper(sf_object *obj, const std::string& key ,std::function<void(sf_json)> func);
    };
}