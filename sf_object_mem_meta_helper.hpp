#pragma once

#include "sf_object_mem_meta_helper.h"
#include "sf_object.hpp"

namespace skyfire
{
    sf_object_mem_meta_helper::sf_object_mem_meta_helper(sf_object *obj,const std::string& key ,std::function<void(std::any)> func) {
        obj->member_set_callback__[key] = func;
    }
}