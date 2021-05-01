#pragma once

#include "option.h"
#include <bits/stdc++.h>

namespace skyfire
{

constexpr int default_co_stack_size = 1024 * 1024 * 2;

struct co_attr_config
{
    size_t      stack_size   = default_co_stack_size;
    bool        shared_stack = false;
    std::string name         = "unamed";
};

using co_attr_option      = option<co_attr_config>;
using co_attr_option_type = typename option<co_attr_config>::OptionFuncType;

namespace co_attr
{
    namespace
    {
        auto set_name = co_attr_option::make_option([](co_attr_config& config, const std::string& name) {
            config.name = name;
        });

        auto set_stack_size = co_attr_option::make_option([](co_attr_config& config, size_t stack_size) {
            config.stack_size = stack_size;
        });

        auto set_shared_stack = co_attr_option::make_option([](co_attr_config& config) {
            config.shared_stack = true;
        });
    }
}

}