#pragma once

#include "sf_nocopy.h"
#include "sf_single_instance.h"
#include "sf_meta_object_factory.h"
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

        bool set_value(std::shared_ptr<sf_object> object, const std::string& key,std::any value);

        bool set_ref(std::shared_ptr<sf_object> object, const std::string& key,std::shared_ptr<sf_object> value);

        bool set_pointer(std::shared_ptr<sf_object> object, const std::string& key,std::shared_ptr<sf_object> value);

        std::shared_ptr<sf_object> get_object(const std::string &key);
    };

}