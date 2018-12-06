#pragma once

#include "sf_nocopy.h"
#include "sf_single_instance.h"
#include "sf_object_factory.h"

namespace skyfire
{
    class sf_object_global_meta_info: public sf_nocopy<>
    {
    public:
        SF_SINGLE_TON(sf_object_global_meta_info)

    private:
        sf_object_global_meta_info() = default;

        sf_object_factory factory__;

    public:
        template <typename T>
        void add_meta(const std::string& class_name);
        template <typename T, typename V>
        bool set_value(const std::shared_ptr<T> object, const std::string& key,V && value);
    };
}