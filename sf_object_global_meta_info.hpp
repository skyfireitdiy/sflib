#pragma once

#include "sf_single_instance.hpp"
#include "sf_object_factory.hpp"
#include "sf_object_global_meta_info.h"

namespace skyfire
{
    template <typename T>
    void sf_object_global_meta_info::add_meta(const std::string &class_name) {
        factory__.reg_object_type<T>(class_name);
    }

    template <typename T, typename V>
    bool sf_object_global_meta_info::set_value(const std::shared_ptr<T> object, const std::string &key, V &&value) {
        if(object->member_set_callback__.count(key) == 0)
        {
            return false;
        }
        object->member_set_callback__[key](std::forward<V>(value));
        return true;
    }
}