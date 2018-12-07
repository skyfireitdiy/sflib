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
        sf_object_mem_meta_helper(sf_object *obj,
                const std::string& key ,std::function<void(std::any)> value_func,
                std::function<void(std::shared_ptr<sf_object>)> ref_func,
                std::function<void(std::shared_ptr<sf_object>)> pointer_func,
                std::function<sf_json()> js_func);
    };
}