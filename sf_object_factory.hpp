#pragma once

#include "sf_object_factory.h"
#include <functional>
#include <utility>


namespace skyfire
{
    template <typename T,typename ... ARGS>
    void sf_object_factory::reg_object_type(const std::string &type) {
        factory__[type] = std::function<std::any(ARGS...)>([](ARGS&&...args) -> std::any{
            return std::make_shared<T>(std::forward<ARGS>(args)...);
        });
    }

    template <typename ... ARGS>
    std::any sf_object_factory::get_object__(const std::string &type, ARGS &&... args) {
        if(factory__.count(type) != 0)
        {
            if(before_create_callback__) {
                before_create_callback__(type);
            }
            auto ret = std::any_cast<std::function<std::any(ARGS...)>>(factory__[type])(std::forward<ARGS>(args)...);
            if(after_create_callback__)
            {
                after_create_callback__(type);
            }
            return ret;
        }
        return nullptr;
    }

    inline void sf_object_factory::set_before_create_callback( std::function<void(const std::string &)> before) {
        before_create_callback__ = std::move(before);
    }

    inline void sf_object_factory::set_after_create_callback( std::function<void(const std::string &)> after) {
        after_create_callback__ = std::move(after);
    }

    inline bool sf_object_factory::has(const std::string &key) const
    {
        return factory__.count(key) != 0;
    }

    template<typename T, typename... ARGS>
    std::shared_ptr<T> sf_object_factory::get_object(const std::string &type, ARGS &&... args) {
        if(factory__.count(type) == 0)
        {
            reg_object_type<T,ARGS...>(type);
        }
        return std::any_cast<std::shared_ptr<T>>(get_object__(type,std::forward<ARGS>(args)...));
    }
}