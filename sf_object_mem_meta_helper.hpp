#pragma once

#include "sf_object_mem_meta_helper.h"
#include "sf_object.hpp"
#include "sf_json.hpp"

namespace skyfire
{
    sf_object_mem_meta_helper::sf_object_mem_meta_helper(sf_object *obj,const std::string& key ,
            std::function<void(std::any)> value_func,
            std::function<void(std::shared_ptr<sf_object>)> ref_func,
            std::function<void(std::shared_ptr<sf_object>)> pointer_func,
            std::function<sf_json()> js_func) {
        if(value_func) {
            obj->mem_value_type__[key] = sf_object::__mem_value_type_t__ ::value;
            obj->member_value_callback__[key] = value_func;
        }else if(ref_func) {
            obj->mem_value_type__[key] = sf_object::__mem_value_type_t__ ::ref;
            obj->member_ref_callback__[key] = ref_func;
        }if(pointer_func) {
            obj->mem_value_type__[key] = sf_object::__mem_value_type_t__ ::pointer;
            obj->member_pointer_callback__[key] = pointer_func;
        }

        obj->to_json_callback__[key] = js_func;
    }
}