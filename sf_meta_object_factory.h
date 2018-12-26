
#pragma once
#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCUnusedGlobalDeclarationInspection"
#include "sf_object.h"

namespace skyfire {
    class sf_object;

    class sf_meta_object_factory final : public sf_nocopy<> {

    private:
        std::unordered_map<std::string, std::function<std::shared_ptr<sf_object>()>> factory__;

        std::function<void(const std::string &)> before_create_callback__ = nullptr;
        std::function<void(const std::string &)> after_create_callback__ = nullptr;

    public:
        template<typename T>
        void reg_object_type(const std::string &type);

        std::shared_ptr<sf_object> get_object(const std::string &type);

        void set_before_create_callback(std::function<void(const std::string &)> before);

        void set_after_create_callback(std::function<void(const std::string &)> after);

        bool has(const std::string &key) const;
    };


}
#pragma clang diagnostic pop