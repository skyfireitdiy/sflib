#pragma once

#include "sf_object_global_meta_info.h"
#include "sf_object.hpp"
#include "sf_meta_object_factory.hpp"

namespace skyfire
{
    template <typename T>
    void sf_object_global_meta_info::add_meta(const std::string &class_name) {
        factory__.reg_object_type<T>(class_name);
    }



    inline std::shared_ptr<sf_object> sf_object_global_meta_info::get_object(const std::string &key)
    {
        return factory__.get_object(key);
    }
}