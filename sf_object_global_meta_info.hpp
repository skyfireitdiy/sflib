#pragma once

#include "sf_single_instance.hpp"
#include "sf_meta_object_factory.hpp"
#include "sf_object_global_meta_info.h"
#include "sf_object.hpp"

namespace skyfire
{
    template <typename T>
    void sf_object_global_meta_info::add_meta(const std::string &class_name) {
        factory__.reg_object_type<T>(class_name);
    }

    bool sf_object_global_meta_info::set_value(std::shared_ptr<sf_object> object, const std::string &key, std::any value) {
        if(object->member_value_callback__.count(key) == 0)
        {
            return false;
        }
        object->member_value_callback__[key](value);
        return true;
    }


    std::shared_ptr<sf_object> sf_object_global_meta_info::get_object(const std::string &key)
    {
        return factory__.get_object(key);
    }

    bool sf_object_global_meta_info::set_ref(std::shared_ptr<sf_object> object, const std::string &key, std::shared_ptr<sf_object> value) {
        if(object->member_ref_callback__.count(key) == 0)
        {
            return false;
        }
        object->member_ref_callback__[key](value);
        return true;
    }


    bool sf_object_global_meta_info::set_pointer(std::shared_ptr<sf_object> object, const std::string &key, std::shared_ptr<sf_object> value) {
        if(object->member_pointer_callback__.count(key) == 0)
        {
            return false;
        }
        object->member_pointer_callback__[key](value);
        return true;
    }

}