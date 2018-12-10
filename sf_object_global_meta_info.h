#pragma once

#include "sf_nocopy.h"
#include "sf_single_instance.h"
#include "sf_meta_object_factory.h"
#include "sf_object.hpp"
#include <memory>
#include <string>


namespace skyfire
{
    class sf_object;

    class sf_object_global_meta_info: public sf_nocopy<>
    {
    public:
        SF_SINGLE_TON(sf_object_global_meta_info)

    private:
        sf_object_global_meta_info() = default;

        sf_meta_object_factory factory__;

    public:
        template <typename T>
        void add_meta(const std::string& class_name);

#define _SET_VALUE_FUNC(func_name, container_name) \
        template<typename T>\
        inline bool func_name(std::shared_ptr<sf_object> object, const std::string &key, T value) {\
            if(object->container_name.count(key) == 0)\
            {\
                return false;\
            }\
            object->container_name[key](value);\
            return true;\
        }\



#define _SET_REF_FUNC(func_name, container_name) \
        template<typename T> \
        inline bool func_name(std::shared_ptr<sf_object> object, const std::string &key, T value) {\
            if(object->container_name.count(key) == 0)\
            {\
                return false;\
            }\
            object->container_name[key](value);\
            return true;\
        }\


        _SET_VALUE_FUNC(set_value,member_value_callback__)

        _SET_REF_FUNC(set_ref,member_ref_callback__)

        _SET_REF_FUNC(set_pointer,member_pointer_callback__)

        _SET_VALUE_FUNC(set_container_value, member_container_value_callback__)

        _SET_REF_FUNC(set_container_ref, member_container_ref_callback__)

        _SET_REF_FUNC(set_container_pointer, member_container_pointer_callback__)


#undef _SET_VALUE_FUNC
#undef _SET_REF_FUNC

        std::shared_ptr<sf_object> get_object(const std::string &key);
    };

}