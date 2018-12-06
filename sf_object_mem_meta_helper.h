#pragma once

class sf_object;

namespace skyfire
{
    class sf_object_mem_meta_helper
    {
    public:
        sf_object_mem_meta_helper(sf_object *obj, const std::string& key ,std::function<void(std::any)> func);
    };
}