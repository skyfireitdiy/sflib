#pragma once

#include "sf_single_instance.hpp"
#include "sf_object_factory.hpp"
#include "sf_object_global_meta_info.h"
#include "sf_object.hpp"

#include <iostream>
using namespace std;

namespace skyfire
{
    template <typename T>
    void sf_object_global_meta_info::add_meta(const std::string &class_name) {
        factory__.reg_object_type<T>(class_name);
    }

    template <typename T>
    bool sf_object_global_meta_info::set_value(std::shared_ptr<T> object, const std::string &key, sf_json value) {
        cout<<"set value " + key + " " << value <<endl;
        if(object->member_set_callback__.count(key) == 0)
        {
            cout<<"callback not found"<<endl;
            return false;
        }
        cout<<"before member callback"<<endl;
        object->member_set_callback__[key](value);
        cout<<"after member callback"<<endl;
        return true;
    }

    template<typename T>
    std::shared_ptr<T> sf_object_global_meta_info::get_object(const std::string &key)
    {
        cout<<"get object "+ key<<endl;
        if(!factory__.has(key)){
            cout<<"key not found"<<endl;
            return nullptr;
        }
        return factory__.get_object<T>(key);
    }
}