#pragma once

#include "sf_nocopy.h"
#include "sf_single_instance.h"
#include "sf_object_factory.h"
#include "sf_json.h"
#include <memory>
#include <string>

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
        template <typename T>
        bool set_value(std::shared_ptr<T> object, const std::string& key,sf_json  value);

        template <typename T>
        std::shared_ptr<T> get_object(const std::string &key);
    };
}