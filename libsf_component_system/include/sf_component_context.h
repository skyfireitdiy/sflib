#pragma once

#include "sf_type.h"
#include "sf_serialize_binary.hpp"
#include <string>
#include <map>

namespace skyfire
{
    struct sf_component_context_t
    {
        std::string name;
        std::map<std::string, byte_array> private_data;
    };

    SF_MAKE_SERIALIZABLE_BINARY(sf_component_context_t,name,private_data)
}