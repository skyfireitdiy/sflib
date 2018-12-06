#pragma once

#include "sf_single_instance.h"
#include "sf_nocopy.h"
#include "sf_object.h"
#include <string>
#include <any>
#include <memory>

namespace skyfire
{
    class sf_object_factory : public sf_nocopy<>
    {

    private:
        std::map<std::string,std::any> factory__;

        std::function<void(const std::string&)> before_create_callback__ = nullptr;
        std::function<void(const std::string&)> after_create_callback__ = nullptr;

    public:
        template <typename T,typename  ... ARGS>
        void reg_object_type(const std::string& type);

        template <typename T,typename ... ARGS>
        std::shared_ptr<T> get_object(const std::string& type, ARGS&& ... args);

        void set_before_create_callback(std::function<void(const std::string&)> before);
        void set_after_create_callback(std::function<void(const std::string&)> after);
        bool has(const std::string &key);
    };
}