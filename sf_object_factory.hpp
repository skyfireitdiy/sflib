#pragma once

#include "sf_object_factory.h"
#include "sf_single_instance.hpp"
#include "sf_object.hpp"


#include <functional>



namespace skyfire
{
    template <typename T,typename ... ARGS>
    void sf_object_factory::reg_object_type(const std::string &type) {
        factory__[type] = std::function([](ARGS...args){
            return std::shared_ptr<T>(new T(std::forward<ARGS>(args)...));
        });
    }

    template <typename T,typename ... ARGS>
    std::shared_ptr<T> sf_object_factory::get_object(const std::string& type,ARGS... args) {
        if(factory__.count(type) != 0)
        {
            if(before_create_callback__) {
                before_create_callback__(type);
            }
            auto ret = std::any_cast<std::function<std::shared_ptr<T>(ARGS...)>>(factory__[type])(std::forward<ARGS>(args)...);
            if(after_create_callback__)
            {
                after_create_callback__(type);
            }
            return ret;
        }
        return nullptr;
    }

    void sf_object_factory::set_before_create_callback(std::function<void(const std::string &)> before) {
        before_create_callback__ = before;
    }

    void sf_object_factory::set_after_create_callback(std::function<void(const std::string &)> after) {
        after_create_callback__ = after;
    }
}