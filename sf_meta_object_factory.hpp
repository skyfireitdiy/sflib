#pragma once

#include "sf_meta_object_factory.h"
#include "sf_object.hpp"

namespace skyfire
{
    template <typename T>
    void sf_meta_object_factory::reg_object_type(const std::string &type) {
        factory__[type] = std::function<std::shared_ptr<sf_object>()>([]() -> std::shared_ptr<sf_object>{
            return std::shared_ptr<T>(new T);
        });
    }
    
    std::shared_ptr<sf_object> sf_meta_object_factory::get_object(const std::string &type) {
        if(factory__.count(type) != 0)
        {
            if(before_create_callback__) {
                before_create_callback__(type);
            }
            auto ret = (factory__[type])();
            if(after_create_callback__)
            {
                after_create_callback__(type);
            }
            return ret;
        }
        return nullptr;
    }

    void sf_meta_object_factory::set_before_create_callback(std::function<void(const std::string &)> before) {
        before_create_callback__ = before;
    }

    void sf_meta_object_factory::set_after_create_callback(std::function<void(const std::string &)> after) {
        after_create_callback__ = after;
    }

    bool sf_meta_object_factory::has(const std::string &key) {
        return factory__.count(key) != 0;
    }
}